/*
  probe.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#ifndef GAMMARAY_PROBE_H
#define GAMMARAY_PROBE_H

#include "gammaray_core_export.h"
#include "signalspycallbackset.h"

#include <common/sourcelocation.h>

#include <QObject>
#include <QList>
#include <QPoint>
#include <QSet>
#include <QVector>

#include <memory>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QItemSelectionModel;
class QModelIndex;
class QThread;
class QTimer;
class QMutex;
class QSignalSpyCallbackSet;
QT_END_NAMESPACE

namespace GammaRay {
class ProbeCreator;
class ObjectListModel;
class ObjectTreeModel;
class MainWindow;
class BenchSuite;
class Server;
class ToolManager;
class ProblemCollector;
class MetaObjectRegistry;
namespace Execution { class Trace; }

/*!
 * Central entity of GammaRay: The probe is tracking the Qt application under test
 *
 * @note The Probe lifetime is strongly coupled with the QCoreApplication lifetime, if there's
 * no QCoreApplication instance, then there's no probe.
 *
 * To get a hold of the probe, call Probe::instance()
 */
class GAMMARAY_CORE_EXPORT Probe : public QObject
{
    Q_OBJECT
public:
    ~Probe() override;

    /*!
     * Returns the current instance of the probe.
     *
     * @note You must hold the object lock when using the probe's object tracking
     * functionality.
     *
     * @sa objectLock()
     */
    static Probe *instance();

    /*!
     * Returns true if the probe is initialized, false otherwise.
     */
    static bool isInitialized();

    ///@cond internal
    static void objectAdded(QObject *obj, bool fromCtor = false);
    static void objectRemoved(QObject *obj);
    ///@endcond

    /*!
     * Returns a list of all QObjects we know about.
     *
     * @note This getter can be used without the object lock. Do acquire the
     * object lock and check the pointer with @e isValidObject though, before
     * dereferencing any of the QObject pointers.
     */
    const QVector<QObject*> &allQObjects() const;

    /*!
     * Returns the object list model.
     * @return a pointer to a QAbstractItemModel instance.
     */
    QAbstractItemModel *objectListModel() const;
    /*!
     * Returns the object tree model.
     * @return a pointer to a QAbstractItemModel instance.
     */
    QAbstractItemModel *objectTreeModel() const;
    /*!
     * Register a model for remote usage.
     * @param objectName Unique identifier for the model, typically in reverse domain notation.
     * @param model The model to register.
     */
    void registerModel(const QString &objectName, QAbstractItemModel *model);
    /*!
     * Install a global event filter.
     * Use this rather than installing the filter manually on QCoreApplication,
     * this will filter out GammaRay-internal events and objects already for you.
     */
    void installGlobalEventFilter(QObject *filter);
    /*!
     * Returns @c true if we haven't been able to track all objects from startup, ie. usually
     * when attaching at runtime.
     * If this is the case, we try to discover QObjects by walking the hierarchy, starting
     * from known singletons, and by watching out for unknown receivers of events.
     * This is far from complete obviously, and plug-ins can help finding more objects, using
     * specific knowledge about the types they are responsible for.
     *
     * Connect to the objectAdded(QObject*) signal on probe(), and call discoverObject(QObject*)
     * for "your" objects.
     *
     * @since 2.5
     */
    bool needsObjectDiscovery() const;
    /*!
     * Notify the probe about QObjects your plug-in can discover by using information about
     * the types it can handle.
     * Only use this if needsObjectDiscovery() returns @c true to maximise performance.
     *
     * @see needsObjectDiscovery()
     * @since 2.0
     */
    void discoverObject(QObject *object);
    /*!
     * Notify the probe about the user selecting one of "your" objects via in-app interaction.
     * If you know the exact position the user interacted with, pass that in as @p pos.
     *
     * @since 2.0
     */
    void selectObject(QObject *object, const QPoint &pos = QPoint());
    void selectObject(QObject *object, const QString &toolId,
                      const QPoint &pos = QPoint());
    /*!
     * Notify the probe about the user selecting one of "your" objects.
     *
     * @since 2.1
     */
    void selectObject(void *object, const QString &typeName);
    /*!
     * Register a signal spy callback set.
     * Signal indexes provided as arguments are mapped to method indexes, ie. argument semantics
     * are the same with Qt4 and Qt5.
     *
     * @since 2.2
     */
    void registerSignalSpyCallbackSet(const SignalSpyCallbackSet &callbacks);

    /*! Returns the source code location @p object was created at. */
    SourceLocation objectCreationSourceLocation(QObject *object) const;
    /*! Returns the entire stack trace for the creation of @p object. */
    Execution::Trace objectCreationStackTrace(QObject *object) const;

    ///@cond internal
    QObject *window() const;
    void setWindow(QObject *window);
    ///@endcond

    MetaObjectRegistry *metaObjectRegistry() const;

    /*!
     * Lock this to check the validity of a QObject
     * and to access it safely afterwards.
     */
    static QMutex *objectLock();

    /*!
     * Check whether @p obj is still valid.
     *
     * @note The objectLock must be locked when this is called!
     */
    bool isValidObject(const QObject *obj) const;

    /*!
     * Determines if the specified QObject belongs to the GammaRay Probe or Window.
     *
     * These objects should not be tracked or shown to the user,
     * hence must be explictly filtered.
     * @param obj is a pointer to a QObject instance.
     *
     * @return true if the specified QObject belongs to the GammaRay Probe
     * or Window; false otherwise.
     */
    bool filterObject(QObject *obj) const;

    ///@cond internal
    static void startupHookReceived();
    template<typename Func> static void executeSignalCallback(const Func &func);
    ///@endcond

    ProblemCollector *problemCollector() const;

signals:
    /*!
     * Emitted when the user selected @p object at position @p pos in the probed application.
     */
    void objectSelected(QObject *object, const QPoint &pos);
    void nonQObjectSelected(void *object, const QString &typeName);

    /*!
     * Emitted for newly created QObjects.
     *
     * Note:
     * - This signal is always emitted from the thread the probe exists in.
     * - The signal is emitted delayed enough for the QObject to have been fully constructed,
     *   i.e. on the next event loop re-entry.
     * - The signal is not emitted if the object has been destroyed completely again meanwhile,
     *   e.g. for objects that only existed on the stack.
     * - For objects created and destroyed in other threads, this signal might be emitted after
     *   its dtor has been entered (in case of short-lived objects), but before it has been finished.
     *   At this point the dtor might have already emitted the destroyed() signal and informed smart
     *   pointers about the destruction. This means you must not rely on any of this for object lifetime
     *   tracking for objects from other threads. Use objectDestroyed() instead.
     * - Do not put @p obj into a QWeakPointer, even if it's exclusively handled in the same thread as
     *   the Probe instance. Qt4 asserts if target code tries to put @p obj into a QSharedPointer afterwards.
     * - The objectLock() is locked.
     */
    void objectCreated(QObject *obj);

    /*!
     * Emitted for destroyed objects.
     *
     * Note:
     * - This signal is emitted from the thread the probe exists in.
     * - The signal is emitted from the end of the QObject dtor, dereferencing @p obj is no longer
     *   safe at this point.
     * - In a multi-threaded application, this signal might reach you way after @p obj has been
     *   destroyed, see isValidObject() for a way to check if the object is still valid before accessing it.
     * - The objectLock() is locked.
     */
    void objectDestroyed(QObject *obj);
    void objectReparented(QObject *obj);

    void aboutToDetach();

protected:
    ///@cond internal
    bool eventFilter(QObject *receiver, QEvent *event) override;
    ///@endcond

private slots:
    void delayedInit();
    void shutdown();

    void processQueuedObjectChanges();
    void handleObjectDestroyed(QObject *obj);

private:
    friend class ProbeCreator;
    friend class BenchSuite;

    /* Returns @c true if we have working hooks in QtCore, that is we are notified reliably
     * about every QObject creation/destruction.
     * @since 2.0
     */
    QT_DEPRECATED bool hasReliableObjectTracking() const;

    void objectFullyConstructed(QObject *obj);

    void queueCreatedObject(QObject *obj);
    void queueDestroyedObject(QObject *obj);
    bool isObjectCreationQueued(QObject *obj) const;
    void purgeChangesForObject(QObject *obj);
    void notifyQueuedObjectChanges();

    void findExistingObjects();

    /*! Check if we are capable of showing widgets. */
    static bool canShowWidgets();
    void showInProcessUi();

    static void createProbe(bool findExisting);
    void resendServerAddress();

    explicit Probe(QObject *parent = nullptr);
    static QAtomicPointer<Probe> s_instance;

    /*! Set up all needed signal spy callbacks. */
    void setupSignalSpyCallbacks();

    ObjectListModel *m_objectListModel;
    ObjectTreeModel *m_objectTreeModel;
    ProblemCollector *m_problemCollector;
    ToolManager *m_toolManager;
    QObject *m_window;
    QSet<const QObject *> m_validObjects;
    MetaObjectRegistry *m_metaObjectRegistry;

    // all delayed object changes need to go through a single queue, as the order is crucial
    struct ObjectChange {
        QObject *obj;
        enum Type {
            Create,
            Destroy
        } type;
    };
    QVector<ObjectChange> m_queuedObjectChanges;

    QList<QObject *> m_pendingReparents;
    QTimer *m_queueTimer;
    QVector<QObject *> m_globalEventFilters;
    QVector<SignalSpyCallbackSet> m_signalSpyCallbacks;

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QSignalSpyCallbackSet *m_previousSignalSpyCallbackSet;
#else
    SignalSpyCallbackSet m_previousSignalSpyCallbackSet;
#endif
    Server *m_server;
};
}

#endif // GAMMARAY_PROBE_H
