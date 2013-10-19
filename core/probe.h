/*
  probe.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include <QObject>
#include <QQueue>
#include <QReadWriteLock>
#include <QSet>
#include <QVector>

class QThread;
class QPoint;
class QTimer;

namespace GammaRay {

class ProbeCreator;
class MetaObjectTreeModel;
class ConnectionModel;
class ObjectListModel;
class ObjectTreeModel;
class ToolModel;
class MainWindow;
class BenchSuite;

class GAMMARAY_CORE_EXPORT Probe : public QObject, public ProbeInterface
{
  Q_OBJECT
  public:
    virtual ~Probe();

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
    static void connectionAdded(QObject *sender, const char *signal,
                                QObject *receiver, const char *method,
                                Qt::ConnectionType type);
    static void connectionRemoved(QObject *sender, const char *signal,
                                  QObject *receiver, const char *method);

    QAbstractItemModel *objectListModel() const;
    QAbstractItemModel *objectTreeModel() const;
    QAbstractItemModel *metaObjectModel() const;
    QAbstractItemModel *connectionModel() const;
    ToolModel *toolModel() const;
    void registerModel(const QString& objectName, QAbstractItemModel* model);
    /*override*/ void installGlobalEventFilter(QObject* filter);
    /*override*/ bool hasReliableObjectTracking() const;
    /*override*/ void discoverObject(QObject* object);
    /*override*/ void selectObject(QObject* object, const QPoint& pos = QPoint());

    QObject *window() const;
    void setWindow(QObject *window);

    QObject *probe() const;

    /**
     * Lock this to check the validity of a QObject
     * and to access it safely afterwards.
     */
    static QReadWriteLock *objectLock();

    /**
     * check whether @p obj is still valid
     *
     * NOTE: the objectLock must be locked when this is called!
     */
    bool isValidObject(QObject *obj) const;

    bool filterObject(QObject *obj) const;

    /** Check if we are capable of showing widgets. */
    static bool canShowWidgets();

    /// internal
    static void startupHookReceived();

  signals:
    /**
     * Emitted when the user selected @p object at position @p pos in the probed application.
     */
    void objectSelected(QObject *object, const QPoint &pos);

    void objectCreated(QObject *obj);
    void objectDestroyed(QObject *obj);
    void objectReparented(QObject *obj);

  protected:
    bool eventFilter(QObject *receiver, QEvent *event);

  private slots:
    void delayedInit();
    void queuedObjectsFullyConstructed();
    void handleObjectDestroyed(QObject *obj);
    void objectParentChanged();

  private:
    friend class ProbeCreator;
    friend class BenchSuite;

    static QThread* filteredThread();

    void objectFullyConstructed(QObject *obj);
    void findExistingObjects();

    static void createProbe(bool findExisting);

    explicit Probe(QObject *parent = 0);
    static QAtomicPointer<Probe> s_instance;

    ObjectListModel *m_objectListModel;
    ObjectTreeModel *m_objectTreeModel;
    MetaObjectTreeModel *m_metaObjectTreeModel;
    ConnectionModel *m_connectionModel;
    ToolModel *m_toolModel;
    QObject *m_window;
    QSet<QObject*> m_validObjects;
    QQueue<QObject*> m_queuedObjects;
    QTimer *m_queueTimer;
    QVector<QObject*> m_globalEventFilters;
};

class GAMMARAY_CORE_EXPORT SignalSlotsLocationStore
{
public:
  /// store the location of @p method
  static void flagLocation(const char *method);

  /// retrieve the location of @p member
  static const char *extractLocation(const char *member);
};

}

#endif // GAMMARAY_PROBE_H
