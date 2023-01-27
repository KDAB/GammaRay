/*
  modelmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "modelmodel.h"
#include <QThread>

using namespace GammaRay;

ModelModel::ModelModel(QObject *parent)
    : ObjectModelBase<QAbstractItemModel>(parent)
{
}

QVariant ModelModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QAbstractItemModel *model = static_cast<QAbstractItemModel *>(index.internalPointer());
    if (!model)
        return QVariant();
    return dataForObject(model, index, role);
}

int ModelModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        if (parent.column() > 0)
            return 0;
        QAbstractItemModel *sourceModel = static_cast<QAbstractItemModel *>(parent.internalPointer());
        Q_ASSERT(sourceModel);
        const QVector<QAbstractProxyModel *> proxies = proxiesForModel(sourceModel);
        return proxies.size();
    }

    return m_models.size();
}

QModelIndex ModelModel::parent(const QModelIndex &child) const
{
    QAbstractItemModel *model = static_cast<QAbstractItemModel *>(child.internalPointer());
    Q_ASSERT(model);
    if (m_models.contains(model))
        return {};

    QAbstractProxyModel *proxy = qobject_cast<QAbstractProxyModel *>(model);
    Q_ASSERT(proxy);
    return indexForModel(proxy->sourceModel());
}

QModelIndex ModelModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid()) {
        QAbstractItemModel *model = static_cast<QAbstractItemModel *>(parent.internalPointer());
        const QVector<QAbstractProxyModel *> proxies = proxiesForModel(model);
        if (proxies.size() <= row)
            return {};
        return createIndex(row, column, proxies.at(row));
    }
    return createIndex(row, column, m_models.at(row));
}

void ModelModel::objectAdded(QObject *obj)
{
    if (obj->thread() != QThread::currentThread()) { // ignore models in secondary threads, they can be deleted at any time
        return;
    }
    QAbstractProxyModel *proxy = qobject_cast<QAbstractProxyModel *>(obj);
    if (proxy) {
        beginResetModel(); // FIXME
        if (proxy->sourceModel())
            m_proxies.push_back(proxy);
        else
            m_models.push_back(proxy);

        connect(proxy, &QAbstractProxyModel::sourceModelChanged, this, [this, proxy]() {
            beginResetModel(); // FIXME
            if (proxy->sourceModel()) {
                const auto i = m_models.indexOf(proxy);
                if (i >= 0)
                    m_models.remove(i);
                m_proxies.push_back(proxy);
            } else {
                const auto i = m_proxies.indexOf(proxy);
                if (i >= 0)
                    m_proxies.remove(i);
                m_models.push_back(proxy);
            }
            endResetModel();
        });
        endResetModel();
        return;
    }

    QAbstractItemModel *model = qobject_cast<QAbstractItemModel *>(obj);
    if (model) {
        beginInsertRows(QModelIndex(), m_models.size(), m_models.size());
        m_models.push_back(model);
        endInsertRows();
    }
}

void ModelModel::objectRemoved(QObject *obj)
{
    // do not dereference!
    QAbstractItemModel *unsafeModelPtr = nullptr;
    memcpy(&unsafeModelPtr, &obj, sizeof(unsafeModelPtr));

    int index = m_models.indexOf(unsafeModelPtr);
    if (index >= 0 && index < m_models.size()) {
        beginRemoveRows(QModelIndex(), index, index);
        m_models.remove(index);
        endRemoveRows();
    }

    for (auto it = m_proxies.begin();
         it != m_proxies.end(); ++it) {
        if (*it == obj) {
            beginResetModel(); // FIXME
            m_proxies.erase(it);
            endResetModel();
            return;
        }
    }
}

QModelIndex ModelModel::indexForModel(QAbstractItemModel *model) const
{
    if (!model)
        return {};
    QAbstractProxyModel *proxy = qobject_cast<QAbstractProxyModel *>(model);
    if (!proxy) {
        Q_ASSERT(m_models.contains(model));
        return index(m_models.indexOf(model), 0, QModelIndex());
    }

    QAbstractItemModel *sourceModel = proxy->sourceModel();
    const QModelIndex parentIndex = indexForModel(sourceModel);
    const QVector<QAbstractProxyModel *> proxies = proxiesForModel(sourceModel);
    Q_ASSERT(proxies.contains(proxy));
    return index(proxies.indexOf(proxy), 0, parentIndex);
}

QVector<QAbstractProxyModel *> ModelModel::proxiesForModel(QAbstractItemModel *model) const
{
    QVector<QAbstractProxyModel *> proxies;
    if (!model)
        return proxies;

    for (QAbstractProxyModel *proxy : m_proxies) {
        if (proxy && proxy->sourceModel() == model)
            proxies.push_back(proxy);
    }

    return proxies;
}
