/*
  probe.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
//krazy:excludeall=null,captruefalse,staticobjects

#include "probe.h"
#include "objectlistmodel.h"
#include "objecttreemodel.h"
#include "metaobjecttreemodel.h"
#include "connectionmodel.h"
#include "toolmodel.h"
#include "readorwritelocker.h"
#include "probesettings.h"
#include "probecontroller.h"
#include "toolpluginmodel.h"
#include "util.h"

#include "tools/modelinspector/modeltest.h"

#include "remote/server.h"
#include "remote/remotemodelserver.h"
#include "remote/serverproxymodel.h"
#include "remote/selectionmodelserver.h"
#include "toolpluginerrormodel.h"
#include "toolfactory.h"

#include <common/objectbroker.h>
#include <common/streamoperators.h>
#include <common/paths.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#include <QApplication>
#endif
#include <QCoreApplication>
#include <QDir>
#include <QLibrary>
#include <QMouseEvent>
#include <QThread>
#include <QTimer>

#include <algorithm>
#include <iostream>
#include <cstdio>

#define IF_DEBUG(x)

using namespace GammaRay;
using namespace std;

QAtomicPointer<Probe> Probe::s_instance = QAtomicPointer<Probe>(0);

namespace GammaRay {

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))

static bool probeConnectCallback(void ** args)
{
  QObject *sender = reinterpret_cast<QObject*>(args[0]);
  const char *signal = reinterpret_cast<const char*>(args[1]);
  QObject *receiver = reinterpret_cast<QObject*>(args[2]);
  const char *method = reinterpret_cast<const char*>(args[3]);
  const Qt::ConnectionType *type = reinterpret_cast<Qt::ConnectionType*>(args[4]);
  Probe::connectionAdded(sender, signal, receiver, method, *type);
  return false;
}

static bool probeDisconnectCallback(void ** args)
{
  QObject *sender = reinterpret_cast<QObject*>(args[0]);
  const char *signal = reinterpret_cast<const char*>(args[1]);
  QObject *receiver = reinterpret_cast<QObject*>(args[2]);
  const char *method = reinterpret_cast<const char*>(args[3]);
  Probe::connectionRemoved(sender, signal, receiver, method);
  return false;
}

#endif // QT_VERSION

static void signal_begin_callback(QObject *caller, int method_index, void **argv)
{
  if (method_index == 0 || Probe::instance()->filterObject(caller))
    return;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  method_index = Util::signalIndexToMethodIndex(caller->metaObject(), method_index);
#endif
  Probe::executeSignalCallback([=](const QSignalSpyCallbackSet &qt_signal_spy_callback_set) {
    if (qt_signal_spy_callback_set.signal_begin_callback) {
      qt_signal_spy_callback_set.signal_begin_callback(caller, method_index, argv);
    }
  });
}

static void signal_end_callback(QObject *caller, int method_index)
{
  if (method_index == 0)
    return;

  ReadOrWriteLocker locker(Probe::objectLock());
  if (!Probe::instance()->isValidObject(caller)) // implies filterObject()
    return; // deleted in the slot

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  method_index = Util::signalIndexToMethodIndex(caller->metaObject(), method_index);
#endif
  Probe::executeSignalCallback([=](const QSignalSpyCallbackSet &qt_signal_spy_callback_set) {
    if (qt_signal_spy_callback_set.signal_end_callback) {
      qt_signal_spy_callback_set.signal_end_callback(caller, method_index);
    }
  });
}

static void slot_begin_callback(QObject *caller, int method_index, void **argv)
{
  if (method_index == 0 || Probe::instance()->filterObject(caller))
    return;

  Probe::executeSignalCallback([=](const QSignalSpyCallbackSet &qt_signal_spy_callback_set) {
    if (qt_signal_spy_callback_set.slot_begin_callback) {
      qt_signal_spy_callback_set.slot_begin_callback(caller, method_index, argv);
    }
  });
}

static void slot_end_callback(QObject *caller, int method_index)
{
  if (method_index == 0)
    return;

  ReadOrWriteLocker locker(Probe::objectLock());
  if (!Probe::instance()->isValidObject(caller)) // implies filterObject()
    return; // deleted in the slot

  Probe::executeSignalCallback([=](const QSignalSpyCallbackSet &qt_signal_spy_callback_set) {
    if (qt_signal_spy_callback_set.slot_end_callback) {
      qt_signal_spy_callback_set.slot_end_callback(caller, method_index);
    }
  });
}

static QItemSelectionModel *selectionModelFactory(QAbstractItemModel *model)
{
  return new SelectionModelServer(model->objectName() + ".selection", model, Probe::instance());
}

}

// useful for debugging, dumps the object and all it's parents
// also useable from GDB!
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
    if (obj) {
      cout << " <- ";
    }
  } while(obj);
  cout << endl;
  cout.flags(oldFlags);
}

struct Listener
{
  Listener()
    : filterThread(0), trackDestroyed(true)
  {
  }

  QThread *filterThread;
  bool trackDestroyed;

  QVector<QObject*> addedBeforeProbeInstance;
};

Q_GLOBAL_STATIC(Listener, s_listener)

// ensures proper information is returned by isValidObject by
// locking it in objectAdded/Removed
class ObjectLock : public QReadWriteLock
{
  public:
    ObjectLock()
      : QReadWriteLock(QReadWriteLock::Recursive)
    {
    }
};
Q_GLOBAL_STATIC(ObjectLock, s_lock)

Probe::Probe(QObject *parent):
  QObject(parent),
  m_objectListModel(new ObjectListModel(this)),
  m_objectTreeModel(new ObjectTreeModel(this)),
  m_metaObjectTreeModel(new MetaObjectTreeModel(this)),
  m_connectionModel(new ConnectionModel(this)),
  m_toolModel(0),
  m_window(0),
  m_queueTimer(new QTimer(this))
{
  Q_ASSERT(thread() == qApp->thread());
  IF_DEBUG(cout << "attaching GammaRay probe" << endl;)

  ProbeSettings::receiveSettings();
  m_toolModel = new ToolModel(this);
  auto sortedToolModel = new ServerProxyModel(this);
  sortedToolModel->setSourceModel(m_toolModel);
  sortedToolModel->setDynamicSortFilter(true);
  sortedToolModel->sort(0);

  Server *server = new Server(this);
  ProbeSettings::sendPort(server->port());

  StreamOperators::registerOperators();
  ObjectBroker::setSelectionModelFactoryCallback(selectionModelFactory);
  ObjectBroker::registerObject<ProbeControllerInterface*>(new ProbeController(this));

  registerModel(QLatin1String("com.kdab.GammaRay.ObjectTree"), m_objectTreeModel);
  registerModel(QLatin1String("com.kdab.GammaRay.ObjectList"), m_objectListModel);
  registerModel(QLatin1String("com.kdab.GammaRay.MetaObjectModel"), m_metaObjectTreeModel);
  registerModel(QLatin1String("com.kdab.GammaRay.ToolModel"), sortedToolModel);
  registerModel(QLatin1String("com.kdab.GammaRay.ConnectionModel"), m_connectionModel);

  m_toolSelectionModel = ObjectBroker::selectionModel(m_toolModel);

  ToolPluginModel *toolPluginModel = new ToolPluginModel(m_toolModel->plugins(), this);
  registerModel(QLatin1String("com.kdab.GammaRay.ToolPluginModel"), toolPluginModel);
  ToolPluginErrorModel *toolPluginErrorModel =
    new ToolPluginErrorModel(m_toolModel->pluginErrors(), this);
  registerModel(QLatin1String("com.kdab.GammaRay.ToolPluginErrorModel"), toolPluginErrorModel);

  if (qgetenv("GAMMARAY_MODELTEST") == "1") {
    new ModelTest(m_objectListModel, m_objectListModel);
    new ModelTest(m_objectTreeModel, m_objectTreeModel);
    new ModelTest(m_connectionModel, m_connectionModel);
    new ModelTest(m_toolModel, m_toolModel);
  }

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  QInternal::registerCallback(QInternal::ConnectCallback, &GammaRay::probeConnectCallback);
  QInternal::registerCallback(QInternal::DisconnectCallback, &GammaRay::probeDisconnectCallback);
#endif

  m_queueTimer->setSingleShot(true);
  m_queueTimer->setInterval(0);
  connect(m_queueTimer, SIGNAL(timeout()),
          this, SLOT(queuedObjectsFullyConstructed()));

  QSignalSpyCallbackSet callbacks;
  callbacks.signal_begin_callback = signal_begin_callback;
  callbacks.signal_end_callback = signal_end_callback;
  callbacks.slot_begin_callback = slot_begin_callback;
  callbacks.slot_end_callback = slot_end_callback;
  m_previousSignalSpyCallbackSet = qt_signal_spy_callback_set;
  if (qt_signal_spy_callback_set.signal_begin_callback ||
      qt_signal_spy_callback_set.signal_end_callback ||
      qt_signal_spy_callback_set.slot_begin_callback ||
      qt_signal_spy_callback_set.slot_end_callback) {
    m_signalSpyCallbacks.push_back(qt_signal_spy_callback_set); // daisy-chain existing callbacks
  }
  qt_register_signal_spy_callbacks(callbacks);
}

Probe::~Probe()
{
  IF_DEBUG(cerr << "detaching GammaRay probe" << endl;)

  qt_register_signal_spy_callbacks(m_previousSignalSpyCallbackSet);

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  QInternal::unregisterCallback(QInternal::ConnectCallback, &GammaRay::probeConnectCallback);
  QInternal::unregisterCallback(QInternal::DisconnectCallback, &GammaRay::probeDisconnectCallback);
#endif

  ObjectBroker::clear();

  s_instance = QAtomicPointer<Probe>(0);
}

QThread *Probe::filteredThread()
{
  return s_listener()->filterThread;
}

void Probe::setWindow(QObject *window)
{
  m_window = window;
}

QObject *Probe::window() const
{
  return m_window;
}

Probe *GammaRay::Probe::instance()
{
  if (!qApp) {
    return NULL;
  }

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  return s_instance;
#else
  return s_instance.load();
#endif
}

bool Probe::isInitialized()
{
  return instance() && qApp;
}

bool Probe::canShowWidgets()
{
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  const QApplication * const qGuiApplication = qobject_cast<const QApplication *>(qApp);
  return qGuiApplication && qGuiApplication->type() != QApplication::Tty;
#else
  return QCoreApplication::instance()->inherits("QApplication");
#endif
}

void Probe::createProbe(bool findExisting)
{
  Q_ASSERT(!isInitialized());

  // first create the probe and its children
  // we must not hold the object lock here as otherwise we can deadlock
  // with other QObject's we create and other threads are using. One
  // example are QAbstractSocketEngine.
  IF_DEBUG(cout << "setting up new probe instance" << endl;)
  s_listener()->filterThread = QThread::currentThread();
  Probe *probe = new Probe;
  s_listener()->filterThread = 0;
  IF_DEBUG(cout << "done setting up new probe instance" << endl;)

  connect(qApp, SIGNAL(aboutToQuit()), probe, SLOT(deleteLater()));

  // now we can get the lock and add items which where added before this point in time
  {
    QWriteLocker lock(s_lock());
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
    if (findExisting) {
      probe->findExistingObjects();
    }
  }

  // eventually initialize the rest
  QMetaObject::invokeMethod(probe, "delayedInit", Qt::QueuedConnection);
}

void Probe::startupHookReceived()
{
  s_listener()->trackDestroyed = false;
}

void Probe::delayedInit()
{
  QCoreApplication::instance()->installEventFilter(this);

  QString appName = qApp->applicationName();
  if (appName.isEmpty() && !qApp->arguments().isEmpty()) {
    appName = qApp->arguments().first().remove(qApp->applicationDirPath());
    if (appName.startsWith('.')) {
        appName = appName.right(appName.length() - 1);
    }
    if (appName.startsWith('/')) {
        appName = appName.right(appName.length() - 1);
    }
  }
  if (appName.isEmpty()) {
    appName = tr("PID %1").arg(qApp->applicationPid());
  }
  Server::instance()->setLabel(appName);

  if (ProbeSettings::value("InProcessUi", false).toBool()) {
    showInProcessUi();
  }
}

void Probe::showInProcessUi()
{
  if (!canShowWidgets()) {
    cerr << "Unable to show in-process UI in a non-QWidget based application." << endl;
    return;
  }

  IF_DEBUG(cout << "creating GammaRay::MainWindow" << endl;)
  s_listener()->filterThread = QThread::currentThread();

  QString path = Paths::currentProbePath();
  if (!path.isEmpty()) {
    path += QDir::separator();
  }
  path += "gammaray_inprocessui";
  QLibrary lib;
  lib.setFileName(path);
  if (!lib.load()) {
    std::cerr << "Failed to load in-process UI module: "
              << qPrintable(lib.errorString())
              << std::endl;
  } else {
    void(*factory)() =
      reinterpret_cast<void(*)()>(lib.resolve("gammaray_create_inprocess_mainwindow"));
    if (!factory) {
      std::cerr << Q_FUNC_INFO << ' ' << qPrintable(lib.errorString()) << endl;
    } else {
      factory();
    }
  }

  s_listener()->filterThread = 0;
  IF_DEBUG(cout << "creation done" << endl;)
}

bool Probe::filterObject(QObject *obj) const
{
  if (obj->thread() != thread()) {
    // shortcut, never filter objects from a different thread
    return false;
  }
  return obj == this || obj == window() ||
          Util::descendantOf(this, obj) ||
          Util::descendantOf(window(), obj);
}

void Probe::registerModel(const QString &objectName, QAbstractItemModel *model)
{
  RemoteModelServer *ms = new RemoteModelServer(objectName, model);
  ms->setModel(model);
  ObjectBroker::registerModelInternal(objectName, model);
}

QAbstractItemModel *Probe::objectListModel() const
{
  return m_objectListModel;
}

QAbstractItemModel *Probe::objectTreeModel() const
{
  return m_objectTreeModel;
}

QAbstractItemModel *Probe::metaObjectModel() const
{
  return m_metaObjectTreeModel;
}

QAbstractItemModel *Probe::connectionModel() const
{
  return m_connectionModel;
}

ToolModel *Probe::toolModel() const
{
  return m_toolModel;
}

QObject *Probe::probe() const
{
  return const_cast<GammaRay::Probe*>(this);
}

bool Probe::isValidObject(QObject *obj) const
{
  ///TODO: can we somehow assert(s_lock().isLocked()) ?!
  return m_validObjects.contains(obj);
}

QReadWriteLock *Probe::objectLock()
{
  return s_lock();
}

void Probe::objectAdded(QObject *obj, bool fromCtor)
{
  QWriteLocker lock(s_lock());
  if (s_listener()->filterThread == obj->thread()) {
    // Ignore
    IF_DEBUG(cout
             << "objectAdded Ignore: "
             << hex << obj
             << (fromCtor ? " (from ctor)" : "") << endl;)
    return;
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
  if (obj->parent() && !instance()->m_validObjects.contains(obj->parent())) {
    objectAdded(obj->parent(), fromCtor);
  }
  Q_ASSERT(!obj->parent() || instance()->m_validObjects.contains(obj->parent()));

  instance()->m_validObjects << obj;
  if (!instance()->hasReliableObjectTracking()) {
    // when we did not use a preload variant that
    // overwrites qt_removeObject we must track object
    // deletion manually
    connect(obj, SIGNAL(destroyed(QObject*)),
            instance(), SLOT(handleObjectDestroyed(QObject*)),
            Qt::DirectConnection);
  }

  if (!fromCtor && obj->parent() && instance()->m_queuedObjects.contains(obj->parent())) {
    // when a child event triggers a call to objectAdded while inside the ctor
    // the parent is already tracked but it's call to objectFullyConstructed
    // was delayed. hence we must do the same for the child for integrity
    fromCtor = true;
  }

  IF_DEBUG(cout << "objectAdded: " << hex << obj
                << (fromCtor ? " (from ctor)" : "")
                << ", p: " << obj->parent() << endl;)

  if (fromCtor) {
    Q_ASSERT(!instance()->m_queuedObjects.contains(obj));
    instance()->m_queuedObjects << obj;
    if (!instance()->m_queueTimer->isActive()) {
      // timers must not be started from a different thread
      QMetaObject::invokeMethod(instance()->m_queueTimer, "start", Qt::AutoConnection);
    }
  } else {
    instance()->objectFullyConstructed(obj);
  }
}

void Probe::queuedObjectsFullyConstructed()
{
  QWriteLocker lock(s_lock());

  IF_DEBUG(cout << Q_FUNC_INFO << " " << m_queuedObjects.size() << endl;)

  // must be called from the main thread via timeout
  Q_ASSERT(QThread::currentThread() == thread());

  // when this is called no object must be in the queue twice
  // otherwise the cleanup procedures failed
  Q_ASSERT(m_queuedObjects.size() == m_queuedObjects.toSet().size());

  foreach (QObject *obj, m_queuedObjects) {
    objectFullyConstructed(obj);
  }

  IF_DEBUG(cout << Q_FUNC_INFO << " done" << endl;)

  m_queuedObjects.clear();
}

void Probe::objectFullyConstructed(QObject *obj)
{
  // must be write locked
  Q_ASSERT(!s_lock()->tryLockForRead());

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

  // QQuickItem has the briliant idea of suppressing child events, so we need an
  // alternative way of detecting reparenting...
  // Without linking to the QtQuick library of course, for extra fun.
  if (obj->inherits("QQuickItem")) {
    connect(obj, SIGNAL(parentChanged(QQuickItem*)), this, SLOT(objectParentChanged()));
  }

  m_metaObjectTreeModel->objectAdded(obj);

  m_toolModel->objectAdded(obj);

  emit objectCreated(obj);
}

void Probe::objectRemoved(QObject *obj)
{
  QWriteLocker lock(s_lock());

  if (!isInitialized()) {
    IF_DEBUG(cout
             << "objectRemoved Before: "
             << hex << obj
             << " have statics: " << s_listener() << endl;)

    if (!s_listener()) {
      return;
    }

    QVector<QObject*> &addedBefore = s_listener()->addedBeforeProbeInstance;
    for (QVector<QObject*>::iterator it = addedBefore.begin(); it != addedBefore.end();) {
      if (*it == obj) {
        it = addedBefore.erase(it);
      } else {
        ++it;
      }
    }
    return;
  }

  IF_DEBUG(cout << "object removed:" << hex << obj << " " << obj->parent() << endl;)

  bool success = instance()->m_validObjects.remove(obj);
  if (!success) {
    // object was not tracked by the probe, probably a gammaray object
    return;
  }

  instance()->m_queuedObjects.removeOne(obj);

  instance()->connectionRemoved(obj, 0, 0, 0);
  instance()->connectionRemoved(0, 0, obj, 0);

  emit instance()->objectDestroyed(obj);
}

void Probe::handleObjectDestroyed(QObject *obj)
{
  objectRemoved(obj);
}

void Probe::objectParentChanged()
{
  if (sender()) {
    emit objectReparented(sender());
  }
}

void Probe::connectionAdded(QObject *sender, const char *signal, QObject *receiver,
                            const char *method, Qt::ConnectionType type)
{
  if (!isInitialized() || !sender || !receiver ||
      s_listener()->filterThread == QThread::currentThread())
  {
    return;
  }

  ReadOrWriteLocker lock(s_lock());
  if (instance()->filterObject(sender) || instance()->filterObject(receiver)) {
    return;
  }

  instance()->m_connectionModel->connectionAdded(sender, signal, receiver, method, type);
}

void Probe::connectionRemoved(QObject *sender, const char *signal,
                              QObject *receiver, const char *method)
{
  if (!isInitialized() || !s_listener() ||
      s_listener()->filterThread == QThread::currentThread())
  {
    return;
  }

  ReadOrWriteLocker lock(s_lock());
  if ((sender && instance()->filterObject(sender)) ||
      (receiver && instance()->filterObject(receiver))) {
    return;
  }

  instance()->m_connectionModel->connectionRemoved(sender, signal, receiver, method);
}

bool Probe::eventFilter(QObject *receiver, QEvent *event)
{
  if (s_listener()->filterThread == receiver->thread()) {
    return QObject::eventFilter(receiver, event);
  }

  if (event->type() == QEvent::ChildAdded || event->type() == QEvent::ChildRemoved) {
    QChildEvent *childEvent = static_cast<QChildEvent*>(event);
    QObject *obj = childEvent->child();

    QWriteLocker lock(s_lock());
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
      } else if (!m_queuedObjects.contains(obj)) {
        // object is known already, just update the position in the tree
        // BUT: only when we did not queue this item before
        IF_DEBUG(cout << "update pos: " << hex << obj << endl;)
        emit objectReparented(obj);
      }
    } else if (tracked) {
      objectRemoved(obj);
    }
  }

  // we have no preloading hooks, so recover all objects we see
  if (!hasReliableObjectTracking() && event->type() != QEvent::ChildAdded &&
      event->type() != QEvent::ChildRemoved && // already handled above
      event->type() != QEvent::Destroy &&
      event->type() != QEvent::WinIdChange && // unsafe since emitted from dtors
      !filterObject(receiver)) {
    QWriteLocker lock(s_lock());
    const bool tracked = m_validObjects.contains(receiver);
    if (!tracked) {
      discoverObject(receiver);
    }
  }

  // filters provided by plugins
  if (!filterObject(receiver)) {
    foreach (QObject *filter, m_globalEventFilters) {
      filter->eventFilter(receiver, event);
    }
  }

  return QObject::eventFilter(receiver, event);
}

void Probe::findExistingObjects()
{
  discoverObject(QCoreApplication::instance());
}

void Probe::discoverObject(QObject *obj)
{
  if (!obj) {
    return;
  }

  QWriteLocker lock(s_lock());
  if (m_validObjects.contains(obj)) {
    return;
  }

  objectAdded(obj);
  foreach (QObject *child, obj->children()) {
    discoverObject(child);
  }
}

void Probe::installGlobalEventFilter(QObject *filter)
{
  Q_ASSERT(!m_globalEventFilters.contains(filter));
  m_globalEventFilters.push_back(filter);
}

bool Probe::hasReliableObjectTracking() const
{
  return !s_listener()->trackDestroyed;
}

void Probe::selectObject(QObject *object, const QPoint &pos)
{
  emit objectSelected(object, pos);

  m_toolSelectionModel->select(m_toolModel->toolForObject(object),
                               QItemSelectionModel::Select |
                               QItemSelectionModel::Clear |
                               QItemSelectionModel::Rows |
                               QItemSelectionModel::Current);
}

void Probe::selectObject(void *object, const QString &typeName)
{
  emit nonQObjectSelected(object, typeName);

  m_toolSelectionModel->select(m_toolModel->toolForObject(object, typeName),
                               QItemSelectionModel::Select |
                               QItemSelectionModel::Clear |
                               QItemSelectionModel::Rows |
                               QItemSelectionModel::Current);
}

void Probe::registerSignalSpyCallbackSet(const QSignalSpyCallbackSet &callbacks)
{
  m_signalSpyCallbacks.push_back(callbacks);
}

template <typename Func>
void Probe::executeSignalCallback(const Func &func)
{
  std::for_each(instance()->m_signalSpyCallbacks.constBegin(),
                instance()->m_signalSpyCallbacks.constEnd(),
                func);
}

//BEGIN: SignalSlotsLocationStore

// taken from qobject.cpp
const int gammaray_flagged_locations_count = 2;
const char *gammaray_flagged_locations[gammaray_flagged_locations_count] = {0};

static int gammaray_idx = 0;

void SignalSlotsLocationStore::flagLocation(const char *method)
{
  gammaray_flagged_locations[gammaray_idx] = method;
  gammaray_idx = (gammaray_idx+1) % gammaray_flagged_locations_count;
}

const char *SignalSlotsLocationStore::extractLocation(const char *member)
{
  for (int i = 0; i < gammaray_flagged_locations_count; ++i) {
    if (member == gammaray_flagged_locations[i]) {
      // signature includes location information after the first null-terminator
      const char *location = member + qstrlen(member) + 1;
      if (*location != '\0') {
        return location;
      }
      return 0;
    }
  }
  return 0;
}

//END

