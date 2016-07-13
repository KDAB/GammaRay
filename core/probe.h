/*
  probe.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "probeinterface.h"
#include "signalspycallbackset.h"

#include <QObject>
#include <QList>
#include <QSet>
#include <QVector>

QT_BEGIN_NAMESPACE
class QItemSelectionModel;
class QModelIndex;
class QThread;
class QPoint;
class QTimer;
class QMutex;
QT_END_NAMESPACE

namespace GammaRay {
class ProbeCreator;
class MetaObjectTreeModel;
class ObjectListModel;
class ObjectTreeModel;
class MainWindow;
class BenchSuite;
class Server;
class ToolManager;

class GAMMARAY_CORE_EXPORT Probe : public QObject, public ProbeInterface
{
    Q_OBJECT
public:
    ~Probe();

    /**
     * NOTE: You must hold the object lock when operating on the instance!
     */
    static Probe *instance();

    /**
     * Returns true if the probe is initialized, false otherwise.
     */
    static bool isInitialized();

    static void objectAdded(QObject *obj, bool fromCtor = false);
    static void objectRemoved(QObject *obj);

    QAbstractItemModel *objectListModel() const Q_DECL_OVERRIDE;
    QAbstractItemModel *objectTreeModel() const Q_DECL_OVERRIDE;
    QAbstractItemModel *metaObjectModel() const;
    void registerModel(const QString &objectName, QAbstractItemModel *model) Q_DECL_OVERRIDE;
    void installGlobalEventFilter(QObject *filter) Q_DECL_OVERRIDE;
    bool needsObjectDiscovery() const Q_DECL_OVERRIDE;
    void discoverObject(QObject *object) Q_DECL_OVERRIDE;
    void selectObject(QObject *object, const QPoint &pos = QPoint()) Q_DECL_OVERRIDE;
    void selectObject(QObject *object, const QString &toolId,
                      const QPoint &pos = QPoint()) Q_DECL_OVERRIDE;
    void selectObject(void *object, const QString &typeName) Q_DECL_OVERRIDE;
    void registerSignalSpyCallbackSet(const SignalSpyCallbackSet &callbacks) Q_DECL_OVERRIDE;

    QObject *window() const;
    void setWindow(QObject *window);

    QObject *probe() const Q_DECL_OVERRIDE;

    /**
     * Lock this to check the validity of a QObject
     * and to access it safely afterwards.
     */
    static QMutex *objectLock();

    /**
     * check whether @p obj is still valid
     *
     * NOTE: the objectLock must be locked when this is called!
     */
    bool isValidObject(QObject *obj) const;

    bool filterObject(QObject *obj) const Q_DECL_OVERRIDE;

    /// internal
    static void startupHookReceived();
    template<typename Func> static void executeSignalCallback(const Func &func);

signals:
    /**
     * Emitted when the user selected @p object at position @p pos in the probed application.
     */
    void objectSelected(QObject *object, const QPoint &pos);
    void nonQObjectSelected(void *object, const QString &typeName);

    /**
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

    /**
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

protected:
    bool eventFilter(QObject *receiver, QEvent *event) Q_DECL_OVERRIDE;

private slots:
    void delayedInit();
    void processQueuedObjectChanges();
    void handleObjectDestroyed(QObject *obj);
    void objectParentChanged();

private:
    friend class ProbeCreator;
    friend class BenchSuite;

    /* Returns @c true if we have working hooks in QtCore, that is we are notified reliably
     * about every QObject creation/destruction.
     * @since 2.0
     */
    bool hasReliableObjectTracking() const;

    void objectFullyConstructed(QObject *obj);

    void queueCreatedObject(QObject *obj);
    void queueDestroyedObject(QObject *obj);
    bool isObjectCreationQueued(QObject *obj) const;
    void purgeChangesForObject(QObject *obj);
    void notifyQueuedObjectChanges();

    void findExistingObjects();

    /** Check if we are capable of showing widgets. */
    static bool canShowWidgets();
    void showInProcessUi();

    static void createProbe(bool findExisting);
    void resendServerAddress();

    explicit Probe(QObject *parent = 0);
    static QAtomicPointer<Probe> s_instance;

    /** Set up all needed signal spy callbacks. */
    void setupSignalSpyCallbacks();

    ObjectListModel *m_objectListModel;
    ObjectTreeModel *m_objectTreeModel;
    MetaObjectTreeModel *m_metaObjectTreeModel;
    ToolManager *m_toolManager;
    QObject *m_window;
    QSet<QObject *> m_validObjects;

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
    SignalSpyCallbackSet m_previousSignalSpyCallbackSet;
    Server *m_server;
};
}

#endif // GAMMARAY_PROBE_H
