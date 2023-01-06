/*
  objectlistmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "objectlistmodel.h"

#include "probe.h"

#include <QThread>
#include <QCoreApplication>

#include <algorithm>
#include <iostream>

using namespace GammaRay;
using namespace std;

ObjectListModel::ObjectListModel(Probe *probe)
    : ObjectModelBase<QAbstractTableModel>(probe)
{
    connect(probe, &Probe::objectCreated,
            this, &ObjectListModel::objectAdded);
    connect(probe, &Probe::objectDestroyed,
            this, &ObjectListModel::objectRemoved);
}

QPair<int, QVariant> ObjectListModel::defaultSelectedItem()
{
    // select the qApp object (if any) in the object model
    return QPair<int, QVariant>(ObjectModel::ObjectRole, QVariant::fromValue<QObject *>(qApp));
}

QVariant ObjectListModel::data(const QModelIndex &index, int role) const
{
    QMutexLocker lock(Probe::objectLock());
    if (index.row() >= 0 && index.row() < m_objects.size()) {
        QObject *obj = m_objects.at(index.row());
        if (Probe::instance()->isValidObject(obj))
            return dataForObject(obj, index, role);
    }
    return QVariant();
}

int ObjectListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return ObjectModelBase<QAbstractTableModel>::columnCount(parent);
}

int ObjectListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_objects.size();
}

void ObjectListModel::objectAdded(QObject *obj)
{
    // see Probe::objectCreated, that promises a valid object in the main thread
    Q_ASSERT(QThread::currentThread() == thread());
    Q_ASSERT(obj);
    Q_ASSERT(Probe::instance()->isValidObject(obj));

    auto it = std::lower_bound(m_objects.begin(), m_objects.end(), obj);
    Q_ASSERT(it == m_objects.end() || *it != obj);

    const int row = std::distance(m_objects.begin(), it);
    Q_ASSERT(row >= 0 && row <= m_objects.size());

    beginInsertRows(QModelIndex(), row, row);
    m_objects.insert(it, obj);
    Q_ASSERT(m_objects.at(row) == obj);
    endInsertRows();
}

void ObjectListModel::objectRemoved(QObject *obj)
{
    Q_ASSERT(thread() == QThread::currentThread());

    auto it = std::lower_bound(m_objects.begin(), m_objects.end(), obj);
    if (it == m_objects.end() || *it != obj) {
        // not found
        return;
    }

    const int row = std::distance(m_objects.begin(), it);
    Q_ASSERT(row >= 0 && row < m_objects.size());
    Q_ASSERT(m_objects.at(row) == obj);

    beginRemoveRows(QModelIndex(), row, row);
    m_objects.erase(it);
    endRemoveRows();
}

const QVector<QObject *> &ObjectListModel::objects() const
{
    return m_objects;
}
