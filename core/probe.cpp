/*
  probe.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/
// krazy:excludeall=null,captruefalse,staticobjects

#include <config-gammaray.h>

#include "probe.h"
#include "enumrepositoryserver.h"
#include "execution.h"
#include "classesiconsrepositoryserver.h"
#include "metaobjectrepository.h"
#include "objectlistmodel.h"
#include "objecttreemodel.h"
#include "probesettings.h"
#include "probecontroller.h"
#include "problemcollector.h"
#include "toolmanager.h"
#include "toolpluginmodel.h"
#include "util.h"
#include "varianthandler.h"
#include "metaobjectregistry.h"
#include "favoriteobject.h"

#include "remote/server.h"
#include "remote/remotemodelserver.h"
#include "remote/serverproxymodel.h"
#include "remote/selectionmodelserver.h"
#include "toolpluginerrormodel.h"
#include "probeguard.h"

#include <common/objectbroker.h>
#include <common/streamoperators.h>
#include <common/paths.h>

#include <QGuiApplication>
#include <QWindow>
#include <QDir>
#include <QLibrary>
#include <QMouseEvent>
#include <QUrl>
#include <QThread>
#include <QTimer>
#include <private/qobject_p.h>
#include <private/qhooks_p.h>
#include <algorithm>
#include <iostream>
#include <cstdio>

#define IF_DEBUG(x)

#ifdef ENABLE_EXPENSIVE_ASSERTS
#define EXPENSIVE_ASSERT(x) Q_ASSERT(x)
#else
#define EXPENSIVE_ASSERT(x)
#endif

using namespace GammaRay;
using namespace std;

QAtomicPointer<Probe> Probe::s_instance = QAtomicPointer<Probe>(nullptr);

namespace GammaRay {
static void signal_begin_callback(QObject *caller, int method_index, void **argv)
{
    // Ignore event dispatcher signals
    if (caller->inherits("QAbstractEventDispatcher"))
        return;

    if (method_index == 0 || !Probe::instance() || Probe::instance()->filterObject(caller))
        return;

    method_index = Util::signalIndexToMethodIndex(caller->metaObject(), method_index);
    Probe::executeSignalCallback([=](const SignalSpyCallbackSet &callbacks) {
        if (callbacks.signalBeginCallback)
            callbacks.signalBeginCallback(caller, method_index, argv);
    });
}

static void signal_end_callback(QObject *caller, int method_index)
{
    if (method_index == 0 || !Probe::instance())
        return;

    QMutexLocker locker(Probe::objectLock());
    if (!Probe::instance())
        return;
    if (!Probe::instance()->isValidObject(caller)) // implies filterObject()
        return; // deleted in the slot
    locker.unlock();

    method_index = Util::signalIndexToMethodIndex(caller->metaObject(), method_index);
    Probe::executeSignalCallback([=](const SignalSpyCallbackSet &callbacks) {
        if (callbacks.signalEndCallback)
            callbacks.signalEndCallback(caller, method_index);
    });
}

static void slot_begin_callback(QObject *caller, int method_index, void **argv)
{
    if (method_index == 0 || !Probe::instance() || Probe::instance()->filterObject(caller))
        return;

    Probe::executeSignalCallback([=](const SignalSpyCallbackSet &callbacks) {
        if (callbacks.slotBeginCallback)
            callbacks.slotBeginCallback(caller, method_index, argv);
    });
}

static void slot_end_callback(QObject *caller, int method_index)
{
    if (method_index == 0 || !Probe::instance())
        return;

    QMutexLocker locker(Probe::objectLock());
    if (!Probe::instance()->isValidObject(caller)) // implies filterObject()
        return; // deleted in the slot
    locker.unlock();

    Probe::executeSignalCallback([=](const SignalSpyCallbackSet &callbacks) {
        if (callbacks.slotEndCallback)
            callbacks.slotEndCallback(caller, method_index);
    });
}

static QItemSelectionModel *selectionModelFactory(QAbstractItemModel *model)
{
    Q_ASSERT(!model->objectName().isEmpty());
    return new SelectionModelServer(model->objectName() + ".selection", model, Probe::instance());
}
}

// useful for debugging, dumps the object and all it's parents
// also usable from GDB!
void dumpObject(QObject *obj)
{
    if (!obj) {
        cout << "QObject(0x0)" << endl;
        return;
    }

    const std::ios::fmtflags oldFlags(cout.flags());
    do {
        cout << obj->metaObject()->className() << "(" << hex << obj << ")";
        obj = obj->parent();
        if (obj)
            cout << " <- ";
    } while (obj);
    cout << endl;
    cout.flags(oldFlags);
}

struct Listener
{
    Listener() = default;

    bool trackDestroyed = true;
    QVector<QObject *> addedBeforeProbeInstance;

    QHash<QObject *, Execution::Trace> constructionBacktracesForObjects;
};

Q_GLOBAL_STATIC(Listener, s_listener)

// ensures proper information is returned by isValidObject by
// locking it in objectAdded/Removed
Q_GLOBAL_STATIC(QRecursiveMutex, s_lock)

Probe::Probe(QObject *parent)
    : QObject(parent)
    , m_objectListModel(new ObjectListModel(this))
    , m_objectTreeModel(new ObjectTreeModel(this))
    , m_window(nullptr)
    , m_metaObjectRegistry(new MetaObjectRegistry(this))
    , m_queueTimer(new QTimer(this))
    , m_server(nullptr)
{
    qputenv("DEBUGINFOD_URLS", QByteArray());

    Q_ASSERT(thread() == qApp->thread());
    IF_DEBUG(cout << "attaching GammaRay probe" << endl;)

    StreamOperators::registerOperators();
    ProbeSettings::receiveSettings();

    m_server = new Server(this);

    ObjectBroker::setSelectionModelFactoryCallback(selectionModelFactory);
    ObjectBroker::registerObject<ProbeControllerInterface *>(new ProbeController(this));
    m_toolManager = new ToolManager(this);
    ObjectBroker::registerObject<ToolManagerInterface *>(m_toolManager);
    ObjectBroker::registerObject<FavoriteObjectInterface *>(new FavoriteObject(this));

    m_problemCollector = new ProblemCollector(this);

    ObjectBroker::registerObject<EnumRepository *>(EnumRepositoryServer::create(this));
    ClassesIconsRepositoryServer::create(this);
    registerModel(QStringLiteral("com.kdab.GammaRay.ObjectTree"), m_objectTreeModel);
    registerModel(QStringLiteral("com.kdab.GammaRay.ObjectList"), m_objectListModel);

    ToolPluginModel *toolPluginModel = new ToolPluginModel(
        m_toolManager->toolPluginManager()->plugins(), this);
    registerModel(QStringLiteral("com.kdab.GammaRay.ToolPluginModel"), toolPluginModel);
    ToolPluginErrorModel *toolPluginErrorModel = new ToolPluginErrorModel(m_toolManager->toolPluginManager()->errors(), this);
    registerModel(QStringLiteral("com.kdab.GammaRay.ToolPluginErrorModel"), toolPluginErrorModel);

    m_queueTimer->setSingleShot(true);
    m_queueTimer->setInterval(0);
    connect(m_queueTimer, &QTimer::timeout,
            this, &Probe::processQueuedObjectChanges);

    m_previousSignalSpyCallbackSet = qt_signal_spy_callback_set.loadRelaxed();

    connect(this, &Probe::objectCreated, m_metaObjectRegistry, &MetaObjectRegistry::objectAdded);
    connect(this, &Probe::objectDestroyed, m_metaObjectRegistry, &MetaObjectRegistry::objectRemoved);
}

Probe::~Probe()
{
    emit aboutToDetach();
    IF_DEBUG(cerr << "detaching GammaRay probe" << endl;)

    // Remove hooks
    qtHookData[QHooks::AddQObject] = 0;
    qtHookData[QHooks::RemoveQObject] = 0;
    qtHookData[QHooks::Startup] = 0;

    qt_register_signal_spy_callbacks(m_previousSignalSpyCallbackSet);

    ObjectBroker::clear();
    ProbeSettings::resetLauncherIdentifier();
    MetaObjectRepository::instance()->clear();
    VariantHandler::clear();

    s_instance = QAtomicPointer<Probe>(nullptr);
}

void Probe::setWindow(QObject *window)
{
    m_window = window;
}

QObject *Probe::window() const
{
    return m_window;
}

MetaObjectRegistry *Probe::metaObjectRegistry() const
{
    return m_metaObjectRegistry;
}

Probe *GammaRay::Probe::instance()
{
    return s_instance.loadRelaxed();
}

bool Probe::isInitialized()
{
    return instance();
}

bool Probe::canShowWidgets()
{
    return QCoreApplication::instance()->inherits("QApplication");
}

void Probe::createProbe(bool findExisting)
{
    Q_ASSERT(qApp);
    Q_ASSERT(!isInitialized());

    // first create the probe and its children
    // we must not hold the object lock here as otherwise we can deadlock
    // with other QObject's we create and other threads are using. One
    // example are QAbstractSocketEngine.
    IF_DEBUG(cout << "setting up new probe instance" << endl;)
    Probe *probe = nullptr;
    {
        ProbeGuard guard;
        probe = new Probe;
    }
    IF_DEBUG(cout << "done setting up new probe instance" << endl;)

    connect(qApp, &QCoreApplication::aboutToQuit, probe, &Probe::shutdown);

    // Our safety net, if there's no call to QCoreApplication::exec() we'll never receive the aboutToQuit() signal
    // Make sure we still cleanup safely after the application instance got destroyed
    connect(qApp, &QObject::destroyed, probe, &Probe::shutdown);

    // now we can get the lock and add items which where added before this point in time
    {
        QMutexLocker lock(s_lock());
        // now we set the instance while holding the lock,
        // all future calls to object{Added,Removed} will
        // act directly on the data structures there instead
        // of using addedBeforeProbeInstance
        // this will only happen _after_ the object lock above is released though
        Q_ASSERT(!instance());

        s_instance = QAtomicPointer<Probe>(probe);

        // add objects to the probe that were tracked before its creation
        foreach (QObject *obj, s_listener()->addedBeforeProbeInstance) {
            objectAdded(obj);
        }
        s_listener()->addedBeforeProbeInstance.clear();

        // try to find existing objects by other means
        if (findExisting)
            probe->findExistingObjects();
    }

    // eventually initialize the rest
    QMetaObject::invokeMethod(probe, "delayedInit", Qt::QueuedConnection);
}

void Probe::resendServerAddress()
{
    Q_ASSERT(isInitialized());
    Q_ASSERT(m_server);
    if (!m_server->isListening()) // already connected
        return;
    ProbeSettings::receiveSettings();
    ProbeSettings::sendServerAddress(m_server->externalAddress());
}

void Probe::startupHookReceived()
{
#ifdef Q_OS_ANDROID
    QDir root = QDir::home();
    root.cdUp();
    Paths::setRootPath(root.absolutePath());
#endif
    s_listener()->trackDestroyed = false;
}

void Probe::delayedInit()
{
    QCoreApplication::instance()->installEventFilter(this);

    QString appName = qApp->applicationName();
    if (appName.isEmpty() && !qApp->arguments().isEmpty()) {
        appName = qApp->arguments().first().remove(qApp->applicationDirPath());
        if (appName.startsWith('.'))
            appName = appName.right(appName.length() - 1);
        if (appName.startsWith('/'))
            appName = appName.right(appName.length() - 1);
    }
    if (appName.isEmpty())
        appName = tr("PID %1").arg(qApp->applicationPid());
    m_server->setLabel(appName);
    // The applicationName might be translated, so let's go with the application file base name
    m_server->setKey(QFileInfo(qApp->applicationFilePath()).completeBaseName());
    m_server->setPid(qApp->applicationPid());

    if (ProbeSettings::value(QStringLiteral("RemoteAccessEnabled"), true).toBool()) {
        const auto serverStarted = m_server->listen();
        if (serverStarted) {
            ProbeSettings::sendServerAddress(m_server->externalAddress());
        } else {
            ProbeSettings::sendServerLaunchError(m_server->errorString());
        }
    }

    if (ProbeSettings::value(QStringLiteral("InProcessUi"), false).toBool())
        showInProcessUi();
}

void Probe::shutdown()
{
    delete this;
}

void Probe::showInProcessUi()
{
    if (!canShowWidgets()) {
        cerr << "Unable to show in-process UI in a non-QWidget based application." << endl;
        return;
    }

    IF_DEBUG(cout << "creating GammaRay::MainWindow" << endl;)
    ProbeGuard guard;

    QLibrary lib;
    for (auto &path : Paths::pluginPaths(QStringLiteral(GAMMARAY_PROBE_ABI))) {
        path += QStringLiteral("/gammaray_inprocessui");
#if defined(GAMMARAY_INSTALL_QT_LAYOUT)
        path += '-';
        path += GAMMARAY_PROBE_ABI;
#else
#if !defined(Q_OS_MAC)
#if defined(QT_DEBUG)
        path += QLatin1String(GAMMARAY_DEBUG_POSTFIX);
#endif
#endif
#endif
        lib.setFileName(path);
        if (lib.load())
            break;
    }

    if (!lib.isLoaded()) {
        std::cerr << "Failed to load in-process UI module: "
                  << qPrintable(lib.errorString())
                  << std::endl;
    } else {
        void (*factory)() = reinterpret_cast<void (*)()>(lib.resolve("gammaray_create_inprocess_mainwindow"));
        if (!factory)
            std::cerr << Q_FUNC_INFO << ' ' << qPrintable(lib.errorString()) << endl;
        else
            factory();
    }

    IF_DEBUG(cout << "creation done" << endl;)
}

bool Probe::filterObject(QObject *obj) const
{
    QSet<QObject *> visitedObjects;
    int iteration = 0;
    QObject *o = obj;
    do {
        if (iteration > 100) {
            // Probably we have a loop in the tree. Do loop detection.
            if (visitedObjects.contains(o)) {
                std::cerr << "We detected a loop in the object tree for object " << o;
                if (!o->objectName().isEmpty())
                    std::cerr << " \"" << qPrintable(o->objectName()) << "\"";
                std::cerr << " (" << o->metaObject()->className() << ")." << std::endl;
                return true;
            }
            visitedObjects << o;
        }
        ++iteration;

        if (o == this || o == window() || (qstrncmp(o->metaObject()->className(), "GammaRay::", 10) == 0)) {
            return true;
        }
        o = o->parent();
    } while (o);
    return false;
}

void Probe::registerModel(const QString &objectName, QAbstractItemModel *model)
{
    auto *ms = new RemoteModelServer(objectName, model);
    ms->setModel(model);
    ObjectBroker::registerModelInternal(objectName, model);
}

QAbstractItemModel *Probe::objectListModel() const
{
    return m_objectListModel;
}

const QVector<QObject *> &Probe::allQObjects() const
{
    return m_objectListModel->objects();
}

QAbstractItemModel *Probe::objectTreeModel() const
{
    return m_objectTreeModel;
}

ProblemCollector *Probe::problemCollector() const
{
    return m_problemCollector;
}

QRecursiveMutex *Probe::objectLock()
{
    return s_lock();
}

/*
 * We need to handle 4 different cases in here:
 * (1) our thread, from ctor:
 * - wait until next event-loop re-entry of our thread
 * - emit objectCreated if object still valid
 * (2) our thread, after ctor:
 * - emit objectCreated right away
 * (3) other thread, from ctor:
 * - wait until next event-loop re-entry in other thread (FIXME: we do not currently do this!!)
 * - post information to our thread
 * (4) other thread, after ctor:
 * - post information to our thread
 * - emit objectCreated there right away if object still valid
 *
 * Pre-conditions: lock may or may not be held already, arbitrary thread
 */
void Probe::objectAdded(QObject *obj, bool fromCtor)
{
    if (obj == nullptr)
        return;
    QMutexLocker lock(s_lock());

    // attempt to ignore objects created by GammaRay itself, especially short-lived ones
    if (fromCtor && ProbeGuard::insideProbe() && obj->thread() == QThread::currentThread())
        return;

    // ignore objects created when global statics are already getting destroyed (on exit)
    if (s_listener.isDestroyed())
        return;


    if (Execution::hasFastStackTrace() && fromCtor) {
        s_listener()->constructionBacktracesForObjects.insert(obj, Execution::stackTrace(32, 2)); // skip 2: this and the hook function calling us
    }

    if (!isInitialized()) {
        IF_DEBUG(cout
                     << "objectAdded Before: "
                     << hex << obj
                     << (fromCtor ? " (from ctor)" : "") << endl;)
        s_listener()->addedBeforeProbeInstance << obj;
        return;
    }

    if (instance()->filterObject(obj)) {
        IF_DEBUG(cout
                     << "objectAdded Filter: "
                     << hex << obj
                     << (fromCtor ? " (from ctor)" : "") << endl;)
        return;
    }

    if (instance()->m_validObjects.contains(obj)) {
        // this happens when we get a child event before the objectAdded call from the ctor
        // or when we add an item from addedBeforeProbeInstance who got added already
        // due to the add-parent-before-child logic
        IF_DEBUG(cout
                     << "objectAdded Known: "
                     << hex << obj
                     << (fromCtor ? " (from ctor)" : "") << endl;)
        return;
    }

    // make sure we already know the parent
    if (obj->parent() && !instance()->m_validObjects.contains(obj->parent()))
        objectAdded(obj->parent(), fromCtor);
    Q_ASSERT(!obj->parent() || instance()->m_validObjects.contains(obj->parent()));

    instance()->m_validObjects << obj;

    if (!fromCtor && obj->parent() && instance()->isObjectCreationQueued(obj->parent())) {
        // when a child event triggers a call to objectAdded while inside the ctor
        // the parent is already tracked but it's call to objectFullyConstructed
        // was delayed. hence we must do the same for the child for integrity
        fromCtor = true;
    }

    IF_DEBUG(cout << "objectAdded: " << hex << obj
                  << (fromCtor ? " (from ctor)" : "")
                  << ", p: " << obj->parent() << endl;)

    if (fromCtor)
        instance()->queueCreatedObject(obj);
    else
        instance()->objectFullyConstructed(obj);
}

// pre-conditions: lock may or may not be held already, our thread
void Probe::processQueuedObjectChanges()
{
    QMutexLocker lock(s_lock());

    IF_DEBUG(cout << Q_FUNC_INFO << " " << m_queuedObjectChanges.size() << endl;)

    // must be called from the main thread via timeout
    Q_ASSERT(QThread::currentThread() == thread());

    const auto queuedObjectChanges = m_queuedObjectChanges; // copy, in case this gets modified while we iterate (which can actually happen)
    for (const auto &change : queuedObjectChanges) {
        switch (change.type) {
        case ObjectChange::Create:
            objectFullyConstructed(change.obj);
            break;
        case ObjectChange::Destroy:
            emit objectDestroyed(change.obj);
            break;
        }
    }

    IF_DEBUG(cout << Q_FUNC_INFO << " done" << endl;)

    m_queuedObjectChanges.clear();

    for (QObject *obj : std::as_const(m_pendingReparents)) {
        if (!isValidObject(obj))
            continue;
        if (filterObject(obj)) // the move might have put it under a hidden parent
            objectRemoved(obj);
        else
            emit objectReparented(obj);
    }
    m_pendingReparents.clear();
}

// pre-condition: lock is held already, our thread
void Probe::objectFullyConstructed(QObject *obj)
{
    Q_ASSERT(thread() == QThread::currentThread());

    if (!m_validObjects.contains(obj)) {
        // deleted already
        IF_DEBUG(cout << "stale fully constructed: " << hex << obj << endl;)
        return;
    }

    if (filterObject(obj)) {
        // when the call was delayed from the ctor construction,
        // the parent might not have been set properly yet. hence
        // apply the filter again
        m_validObjects.remove(obj);
        IF_DEBUG(cout << "now filtered fully constructed: " << hex << obj << endl;)
        return;
    }

    IF_DEBUG(cout << "fully constructed: " << hex << obj << endl;)

    // ensure we know all our ancestors already
    for (QObject *parent = obj->parent(); parent; parent = parent->parent()) {
        if (!m_validObjects.contains(parent)) {
            objectAdded(parent); // will also handle any further ancestors
            break;
        }
    }
    Q_ASSERT(!obj->parent() || m_validObjects.contains(obj->parent()));

    m_toolManager->objectAdded(obj);
    emit objectCreated(obj);
}

/*
 * We have two cases to consider here:
 * (1) our thread:
 * - emit objectDestroyed() right away
 * (2) other thread:
 * - post information to our thread, emit objectDestroyed() there
 *
 * pre-conditions: arbitrary thread, lock may or may not be held already
 */
void Probe::objectRemoved(QObject *obj)
{
    QMutexLocker lock(s_lock());

    if (!isInitialized()) {
        IF_DEBUG(cout
                     << "objectRemoved Before: "
                     << hex << obj
                     << " have statics: " << s_listener() << endl;)

        if (!s_listener())
            return;

        QVector<QObject *> &addedBefore = s_listener()->addedBeforeProbeInstance;
        for (auto it = addedBefore.begin(); it != addedBefore.end();) {
            if (*it == obj)
                it = addedBefore.erase(it);
            else
                ++it;
        }
        return;
    }

    IF_DEBUG(cout << "object removed:" << hex << obj << " " << obj->parent() << endl;)

    bool success = instance()->m_validObjects.remove(obj);
    if (!success) {
        // object was not tracked by the probe, probably a gammaray object
        EXPENSIVE_ASSERT(!instance()->isObjectCreationQueued(obj));
        return;
    }

    instance()->purgeChangesForObject(obj);
    EXPENSIVE_ASSERT(!instance()->isObjectCreationQueued(obj));

    if (instance()->thread() == QThread::currentThread())
        emit instance()->objectDestroyed(obj);
    else
        instance()->queueDestroyedObject(obj);
}

void Probe::handleObjectDestroyed(QObject *obj)
{
    objectRemoved(obj);
}

// pre-condition: we have the lock, arbitrary thread
void Probe::queueCreatedObject(QObject *obj)
{
    EXPENSIVE_ASSERT(!isObjectCreationQueued(obj));

    ObjectChange c;
    c.obj = obj;
    c.type = ObjectChange::Create;
    m_queuedObjectChanges.push_back(c);
    notifyQueuedObjectChanges();
}

// pre-condition: we have the lock, arbitrary thread
void Probe::queueDestroyedObject(QObject *obj)
{
    ObjectChange c;
    c.obj = obj;
    c.type = ObjectChange::Destroy;
    m_queuedObjectChanges.push_back(c);
    notifyQueuedObjectChanges();
}

// pre-condition: we have the lock, arbitrary thread
bool Probe::isObjectCreationQueued(QObject *obj) const
{
    return std::find_if(m_queuedObjectChanges.begin(), m_queuedObjectChanges.end(),
                        [obj](const ObjectChange &c) {
                            return c.obj == obj && c.type == Probe::ObjectChange::Create;
                        })
        != m_queuedObjectChanges.end();
}

// pre-condition: we have the lock, arbitrary thread
void Probe::purgeChangesForObject(QObject *obj)
{
    for (int i = 0; i < m_queuedObjectChanges.size(); ++i) {
        if (m_queuedObjectChanges.at(i).obj == obj
            && m_queuedObjectChanges.at(i).type == ObjectChange::Create) {
            m_queuedObjectChanges.remove(i);
            return;
        }
    }
}

// pre-condition: we have the lock, arbitrary thread
void Probe::notifyQueuedObjectChanges()
{
    if (m_queueTimer->isActive())
        return;

    if (thread() == QThread::currentThread()) {
        m_queueTimer->start();
    } else {
        static QMetaMethod m;
        if (m.methodIndex() < 0) {
            const auto idx = QTimer::staticMetaObject.indexOfMethod("start()");
            Q_ASSERT(idx >= 0);
            m = QTimer::staticMetaObject.method(idx);
            Q_ASSERT(m.methodIndex() >= 0);
        }
        m.invoke(m_queueTimer, Qt::QueuedConnection);
    }
}

bool Probe::eventFilter(QObject *receiver, QEvent *event)
{
    if (ProbeGuard::insideProbe() && receiver->thread() == QThread::currentThread())
        return QObject::eventFilter(receiver, event);

    if (event->type() == QEvent::ChildAdded || event->type() == QEvent::ChildRemoved) {
        QChildEvent *childEvent = static_cast<QChildEvent *>(event);
        QObject *obj = childEvent->child();

        QMutexLocker lock(s_lock());
        const bool tracked = m_validObjects.contains(obj);
        const bool filtered = filterObject(obj);

        IF_DEBUG(cout << "child event: " << hex << obj << ", p: " << obj->parent() << dec
                      << ", tracked: " << tracked
                      << ", filtered: " << filtered
                      << ", type: " << (childEvent->added() ? "added" : "removed") << endl;)

        if (!filtered && childEvent->added()) {
            if (!tracked) {
                // was not tracked before, add to all models
                // child added events are sent before qt_addObject is called,
                // so we assumes this comes from the ctor
                objectAdded(obj, true);
            } else if (!isObjectCreationQueued(obj) && !isObjectCreationQueued(obj->parent()) && isValidObject(obj->parent())) {
                // object is known already, just update the position in the tree
                // BUT: only when we did not queue this item before
                IF_DEBUG(cout << "update pos: " << hex << obj << endl;)
                m_pendingReparents.removeAll(obj);
                emit objectReparented(obj);
            } else if (!isValidObject(obj->parent())) {
                objectAdded(obj->parent());
                m_pendingReparents.push_back(obj);
                notifyQueuedObjectChanges();
            }
        } else if (tracked) {
            // defer processing this until we know its final location
            m_pendingReparents.push_back(obj);
            notifyQueuedObjectChanges();
        }
    }

    // widget only unfortunately, but more precise than ChildAdded/Removed...
    if (event->type() == QEvent::ParentChange) {
        QMutexLocker lock(s_lock());
        const bool tracked = m_validObjects.contains(receiver);
        const bool filtered = filterObject(receiver);
        const bool parentTracked = m_validObjects.contains(receiver->parent());

        if (!filtered && tracked && !isObjectCreationQueued(receiver)
            && !isObjectCreationQueued(receiver->parent()) && parentTracked) {
            m_pendingReparents.removeAll(receiver);
            emit objectReparented(receiver);
        } else if (!parentTracked) {
            objectAdded(receiver->parent());
            m_pendingReparents.push_back(receiver);
            notifyQueuedObjectChanges();
        }
    }

    // we have no preloading hooks, so recover all objects we see
    if (needsObjectDiscovery() && event->type() != QEvent::ChildAdded
        && event->type() != QEvent::ChildRemoved
        && event->type() != QEvent::ParentChange // already handled above
        && event->type() != QEvent::Destroy
        && event->type() != QEvent::WinIdChange // unsafe since emitted from dtors
        && !filterObject(receiver)) {
        QMutexLocker lock(s_lock());
        const bool tracked = m_validObjects.contains(receiver);
        if (!tracked)
            discoverObject(receiver);
    }

    // filters provided by plugins
    if (!filterObject(receiver)) {
        for (QObject *filter : std::as_const(m_globalEventFilters)) {
            filter->eventFilter(receiver, event);
        }
    }

    return QObject::eventFilter(receiver, event);
}

void Probe::findExistingObjects()
{
    discoverObject(QCoreApplication::instance());

    if (auto guiApp = qobject_cast<QGuiApplication *>(QCoreApplication::instance())) {
        foreach (auto window, guiApp->allWindows()) {
            discoverObject(window);
        }
    }
}

void Probe::discoverObject(QObject *object)
{
    if (!object)
        return;

    QMutexLocker lock(s_lock());
    if (m_validObjects.contains(object))
        return;

    objectAdded(object);
    foreach (QObject *child, object->children()) {
        discoverObject(child);
    }
}

void Probe::installGlobalEventFilter(QObject *filter)
{
    Q_ASSERT(!m_globalEventFilters.contains(filter));
    m_globalEventFilters.push_back(filter);
}

bool Probe::needsObjectDiscovery()
{
    return s_listener()->trackDestroyed;
}

bool Probe::hasReliableObjectTracking()
{
    return true; // qHooks available, which works independent of the injector used
}

void Probe::selectObject(QObject *object, const QPoint &pos)
{
    const auto tools = m_toolManager->toolsForObject(object);
    m_toolManager->selectTool(tools.value(0));
    emit objectSelected(object, pos);
}

void Probe::selectObject(QObject *object, const QString &toolId, const QPoint &pos)
{
    if (!m_toolManager->hasTool(toolId)) {
        std::cerr << "Invalid tool id: " << qPrintable(toolId) << std::endl;
        return;
    }

    m_toolManager->selectTool(toolId);
    emit objectSelected(object, pos);
}

void Probe::selectObject(void *object, const QString &typeName)
{
    const auto tools = m_toolManager->toolsForObject(object, typeName);
    const QString toolId = tools.value(0);

    if (!m_toolManager->hasTool(toolId)) {
        std::cerr << "Invalid tool id: " << qPrintable(toolId) << std::endl;
        return;
    }

    m_toolManager->selectTool(tools.value(0));
    emit nonQObjectSelected(object, typeName);
}

void Probe::markObjectAsFavorite(QObject *object)
{
    {
        QMutexLocker locker(Probe::objectLock());
        if (!Probe::instance()->isValidObject(object))
            return; // deleted in the slot
    }

    Q_EMIT objectFavorited(object);
}

void Probe::removeObjectAsFavorite(QObject *object)
{
    {
        QMutexLocker locker(Probe::objectLock());
        if (!Probe::instance()->isValidObject(object))
            return; // deleted in the slot
    }
    Q_EMIT objectUnfavorited(object);
}

void Probe::registerSignalSpyCallbackSet(const SignalSpyCallbackSet &callbacks)
{
    if (callbacks.isNull())
        return;
    m_signalSpyCallbacks.push_back(callbacks);
    setupSignalSpyCallbacks();
}

void Probe::setupSignalSpyCallbacks()
{
    // memory management is with us for Qt >= 5.14, therefore static here!
    static QSignalSpyCallbackSet cbs = { nullptr, nullptr, nullptr, nullptr };
    foreach (const auto &it, m_signalSpyCallbacks) {
        if (it.signalBeginCallback)
            cbs.signal_begin_callback = signal_begin_callback;
        if (it.signalEndCallback)
            cbs.signal_end_callback = signal_end_callback;
        if (it.slotBeginCallback)
            cbs.slot_begin_callback = slot_begin_callback;
        if (it.slotEndCallback)
            cbs.slot_end_callback = slot_end_callback;
    }
    qt_register_signal_spy_callbacks(&cbs);
}

template<typename Func>
void Probe::executeSignalCallback(const Func &func)
{
    std::for_each(instance()->m_signalSpyCallbacks.constBegin(),
                  instance()->m_signalSpyCallbacks.constEnd(),
                  func);
}

SourceLocation Probe::objectCreationSourceLocation(const QObject *object)
{
    QObject *key = const_cast<QObject *>(object);
    if (!s_listener()->constructionBacktracesForObjects.contains(key)) {
        IF_DEBUG(std::cout << "No backtrace for object available" << object << "." << std::endl;)
        return SourceLocation();
    }

    const auto &st = s_listener()->constructionBacktracesForObjects.value(key);
    int distanceToQObject = 0;

    const QMetaObject *metaObject = object->metaObject();
    while (metaObject && metaObject != &QObject::staticMetaObject) {
        distanceToQObject++;
        metaObject = metaObject->superClass();
    }

    const auto frame = Execution::resolveOne(st, distanceToQObject + 1);
    return frame.location;
}

Execution::Trace Probe::objectCreationStackTrace(QObject *object)
{
    return s_listener()->constructionBacktracesForObjects.value(object);
}
