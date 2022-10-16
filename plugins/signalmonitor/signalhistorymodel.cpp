/*
  signalhistorymodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "signalhistorymodel.h"
#include "relativeclock.h"
#include "signalmonitorcommon.h"

#include <core/util.h>
#include <core/probe.h>

#include <common/metatypedeclarations.h>
#include <common/objectid.h>

#include <QLocale>
#include <QMutex>
#include <QSet>
#include <QThread>
#include <QTimer>

using namespace GammaRay;

/// Tries to reuse an already existing instances of @p str by checking
/// a global string pool. If no instance of @p str is interned yet the
/// string will be added to the pool.
template<typename T>
static T internString(const T &str)
{
    static QSet<T> pool;

    // Check if the pool already contains the string...
    auto it = pool.find(str);
    // ...and return it if possible.
    if (it != pool.end())
        return *it;

    // Otherwise add the string to the pool.
    pool.insert(str);
    return str;
}

static SignalHistoryModel *s_historyModel = nullptr;

static void signal_begin_callback(QObject *caller, int method_index, void **argv)
{
    Q_UNUSED(argv);
    if (s_historyModel) {
        const int signalIndex = method_index + 1; // offset 1, so unknown signals end up at 0
        static const QMetaMethod m = s_historyModel->metaObject()->method(
            s_historyModel->metaObject()->indexOfMethod("onSignalEmitted(QObject*,int)"));
        Q_ASSERT(m.isValid());
        m.invoke(s_historyModel, Qt::AutoConnection, Q_ARG(QObject *, caller),
                 Q_ARG(int, signalIndex));
    }
}

SignalHistoryModel::SignalHistoryModel(Probe *probe, QObject *parent)
    : QAbstractTableModel(parent)
{
    connect(probe, &Probe::objectCreated, this, &SignalHistoryModel::onObjectAdded);
    connect(probe, &Probe::objectDestroyed, this, &SignalHistoryModel::onObjectRemoved);
    connect(probe, &Probe::objectFavorited, this, &SignalHistoryModel::onObjectFavorited);
    connect(probe, &Probe::objectUnfavorited, this, &SignalHistoryModel::onObjectUnfavorited);

    SignalSpyCallbackSet spy;
    spy.signalBeginCallback = signal_begin_callback;
    probe->registerSignalSpyCallbackSet(spy);

    s_historyModel = this;

    m_delayInsertTimer = new QTimer(this);
    m_delayInsertTimer->setInterval(100);
    m_delayInsertTimer->setSingleShot(true);
    connect(m_delayInsertTimer, &QTimer::timeout, this, &SignalHistoryModel::insertPendingObjects);
}

SignalHistoryModel::~SignalHistoryModel()
{
    s_historyModel = nullptr;
    qDeleteAll(m_objectsToBeInserted);
    qDeleteAll(m_tracedObjects);
}

int SignalHistoryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_tracedObjects.size();
}

int SignalHistoryModel::columnCount(const QModelIndex &) const
{
    return 3;
}

SignalHistoryModel::Item *SignalHistoryModel::item(const QModelIndex &index) const
{
    if (!index.isValid())
        return nullptr;
    return m_tracedObjects.at(index.row());
}

QVariant SignalHistoryModel::data(const QModelIndex &index, int role) const
{
    switch (static_cast<ColumnId>(index.column())) {
    case ObjectColumn:
        if (role == Qt::DisplayRole)
            return item(index)->objectName;
        if (role == Qt::ToolTipRole)
            return tr("Address: %1").arg(Util::addressToString(item(index)->object));
        if (role == ObjectModel::DecorationIdRole)
            return item(index)->decorationId;
        if (role == ObjectModel::ObjectIdRole && item(index)->object)
            return QVariant::fromValue(ObjectId(item(index)->object));

        break;

    case TypeColumn:
        if (role == Qt::DisplayRole)
            return item(index)->objectType;
        break;

    case EventColumn:
        if (role == EventsRole)
            return QVariant::fromValue(item(index)->events);
        if (role == StartTimeRole)
            return item(index)->startTime;
        if (role == EndTimeRole)
            return item(index)->endTime();
        if (role == SignalMapRole)
            return QVariant::fromValue(item(index)->signalNames);

        break;
    }

    if (role == ObjectModel::IsFavoriteRole) {
        return m_favorites.contains(item(index)->object);
    }

    return QVariant();
}

QVariant SignalHistoryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case ObjectColumn:
            return tr("Object");
        case TypeColumn:
            return tr("Type");
        case EventColumn:
            return tr("Events");
        }
    }

    return QVariant();
}

QMap<int, QVariant> SignalHistoryModel::itemData(const QModelIndex &index) const
{
    QMap<int, QVariant> d = QAbstractItemModel::itemData(index);
    d.insert(EventsRole, data(index, EventsRole));
    d.insert(StartTimeRole, data(index, StartTimeRole));
    d.insert(EndTimeRole, data(index, EndTimeRole));
    d.insert(SignalMapRole, data(index, SignalMapRole));
    d.insert(ObjectModel::ObjectIdRole, data(index, ObjectModel::ObjectIdRole));
    d.insert(ObjectModel::DecorationIdRole, data(index, ObjectModel::DecorationIdRole));
    d.insert(ObjectModel::IsFavoriteRole, data(index, ObjectModel::IsFavoriteRole));
    return d;
}

void SignalHistoryModel::insertPendingObjects()
{
    if (m_objectsToBeInserted.empty())
        return;

    beginInsertRows(QModelIndex(), ( int )m_tracedObjects.size(), m_tracedObjects.size() + ( int )m_objectsToBeInserted.size() - 1);

    int oldSize = m_tracedObjects.size();
    m_tracedObjects.append(std::move(m_objectsToBeInserted));
    for (int i = oldSize; i < m_tracedObjects.size(); ++i) {
        m_itemIndex.insert(m_tracedObjects[i]->object, i);
    }
    m_objectsToBeInserted.clear();

    endInsertRows();
}

void SignalHistoryModel::onObjectAdded(QObject *object)
{
    Q_ASSERT(thread() == QThread::currentThread());

    // blacklist event dispatchers
    if (qstrncmp(object->metaObject()->className(), "QPAEventDispatcher", 18) == 0
        || qstrncmp(object->metaObject()->className(), "QGuiEventDispatcher", 19) == 0
        || qstrncmp(object->metaObject()->className(), "QEventDispatcher", 16) == 0)
        return;

    m_objectsToBeInserted << new Item(object);
    if (!m_delayInsertTimer->isActive())
        m_delayInsertTimer->start();
}

void SignalHistoryModel::onObjectRemoved(QObject *object)
{
    Q_ASSERT(thread() == QThread::currentThread());

    {
        auto it = std::find_if(m_objectsToBeInserted.begin(), m_objectsToBeInserted.end(), [object](Item *item) {
            return item->object == object;
        });
        if (it != m_objectsToBeInserted.end()) {
            delete *it;
            m_objectsToBeInserted.erase(it);
            return;
        }
    }

    m_favorites.remove(object);
    const auto it = m_itemIndex.find(object);
    if (it == m_itemIndex.end())
        return;
    const int itemIndex = *it;
    m_itemIndex.erase(it);

    Item *data = m_tracedObjects.at(itemIndex);
    Q_ASSERT(data->object == object);
    data->object = nullptr;
    emit dataChanged(index(itemIndex, ObjectColumn), index(itemIndex, ObjectColumn)); // for ObjectIdRole
    emit dataChanged(index(itemIndex, EventColumn), index(itemIndex, EventColumn));
}

void SignalHistoryModel::onObjectFavorited(QObject *object)
{
    auto it = m_itemIndex.find(object);
    if (it == m_itemIndex.end())
        return;
    const int itemIndex = *it;
    m_favorites.insert(object);
    emit dataChanged(index(itemIndex, ObjectColumn), index(itemIndex, ObjectColumn), { ObjectModel::IsFavoriteRole });
}

void SignalHistoryModel::onObjectUnfavorited(QObject *object)
{
    auto it = m_itemIndex.find(object);
    if (it == m_itemIndex.end())
        return;
    const int itemIndex = *it;
    Q_ASSERT(m_favorites.contains(object));
    m_favorites.remove(object);
    emit dataChanged(index(itemIndex, ObjectColumn), index(itemIndex, ObjectColumn), { ObjectModel::IsFavoriteRole });
}

void SignalHistoryModel::onSignalEmitted(QObject *sender, int signalIndex)
{
    Q_ASSERT(thread() == QThread::currentThread());
    const qint64 timestamp = RelativeClock::sinceAppStart()->mSecs();

    const auto it = m_itemIndex.constFind(sender);
    if (it == m_itemIndex.constEnd())
        return;
    const int itemIndex = *it;

    Item *data = m_tracedObjects.at(itemIndex);
    Q_ASSERT(data->object == sender);
    // ensure the item is known
    if (signalIndex > 0 && !data->signalNames.contains(signalIndex)) {
        // protect dereferencing of sender here
        QMutexLocker lock(Probe::objectLock());
        if (!Probe::instance()->isValidObject(sender))
            return;
        const QByteArray signalName = sender->metaObject()->method(signalIndex - 1).methodSignature();
        data->signalNames.insert(signalIndex, internString(signalName));
    }

    data->events.push_back((timestamp << 16) | signalIndex);
    emit dataChanged(index(itemIndex, EventColumn), index(itemIndex, EventColumn));
}

SignalHistoryModel::Item::Item(QObject *obj)
    : object(obj)
    , startTime(RelativeClock::sinceAppStart()->mSecs())
{
    objectName = Util::shortDisplayString(object);
    objectType = internString(QByteArray(obj->metaObject()->className()));
    decorationId = Util::iconIdForObject(object);
}

qint64 SignalHistoryModel::Item::endTime() const
{
    if (object)
        return -1; // still alive
    if (!events.isEmpty())
        return timestamp(events.size() - 1);

    return startTime;
}
