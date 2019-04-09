/*
  qt3dentitytreemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "qt3dentitytreemodel.h"

#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QEntity>

#include <algorithm>

using namespace GammaRay;

Qt3DEntityTreeModel::Qt3DEntityTreeModel(QObject *parent)
    : ObjectModelBase<QAbstractItemModel>(parent)
    , m_engine(nullptr)
{
}

Qt3DEntityTreeModel::~Qt3DEntityTreeModel()
{
}

void Qt3DEntityTreeModel::setEngine(Qt3DCore::QAspectEngine *engine)
{
    beginResetModel();
    clear();
    m_engine = engine;
    populateFromEntity(engine->rootEntity().data());
    endResetModel();
}

void Qt3DEntityTreeModel::clear()
{
    for (auto it = m_childParentMap.constBegin(); it != m_childParentMap.constEnd(); ++it)
        disconnectEntity(it.key());
    m_childParentMap.clear();
    m_parentChildMap.clear();
}

void Qt3DEntityTreeModel::populateFromNode(Qt3DCore::QNode *node)
{
    // the entity tree can have intermediate nodes, so we need to do this
    // recursively without a depth limit
    auto entity = qobject_cast<Qt3DCore::QEntity*>(node);
    if (entity) {
        populateFromEntity(entity);
    } else {
        foreach (auto child, node->childNodes())
            populateFromNode(child);
    }
}

void Qt3DEntityTreeModel::populateFromEntity(Qt3DCore::QEntity *entity)
{
    if (!entity)
        return;

    m_childParentMap[entity] = entity->parentEntity();
    m_parentChildMap[entity->parentEntity()].push_back(entity);
    connectEntity(entity);

    foreach (auto child, entity->childNodes()) {
        populateFromNode(child);
    }

    auto &children = m_parentChildMap[entity->parentEntity()];
    std::sort(children.begin(), children.end());
}

QVariant Qt3DEntityTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_engine)
        return QVariant();

    auto entity = reinterpret_cast<Qt3DCore::QEntity *>(index.internalPointer());
    if (role == ObjectModel::ObjectIdRole)
        return QVariant::fromValue(ObjectId(entity));
    else if (role == Qt::CheckStateRole && index.column() == 0)
        return entity->isEnabled() ? Qt::Checked : Qt::Unchecked;

    return dataForObject(entity, index, role);
}

int Qt3DEntityTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!m_engine || !m_engine->rootEntity())
        return 0;

    auto parentEntity = reinterpret_cast<Qt3DCore::QEntity *>(parent.internalPointer());
    return m_parentChildMap.value(parentEntity).size();
}

QModelIndex Qt3DEntityTreeModel::parent(const QModelIndex &child) const
{
    auto childEntity = reinterpret_cast<Qt3DCore::QEntity *>(child.internalPointer());
    return indexForEntity(m_childParentMap.value(childEntity));
}

QModelIndex Qt3DEntityTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    auto parentEntity = reinterpret_cast<Qt3DCore::QEntity *>(parent.internalPointer());
    const auto children = m_parentChildMap.value(parentEntity);
    if (row < 0 || column < 0 || row >= children.size() || column >= columnCount())
        return QModelIndex();
    return createIndex(row, column, children.at(row));
}

Qt::ItemFlags Qt3DEntityTreeModel::flags(const QModelIndex &index) const
{
    auto baseFlags = QAbstractItemModel::flags(index);
    if (index.isValid() && index.column() == 0)
        return baseFlags | Qt::ItemIsUserCheckable;
    return baseFlags;
}

bool Qt3DEntityTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!m_engine || !index.isValid() || role != Qt::CheckStateRole || index.column() != 0)
        return false;

    auto entity = reinterpret_cast<Qt3DCore::QEntity*>(index.internalPointer());
    entity->setEnabled(value.toInt() == Qt::Checked);
    emit dataChanged(index, index);
    return true;
}

QModelIndex Qt3DEntityTreeModel::indexForEntity(Qt3DCore::QEntity *entity) const
{
    if (!entity)
        return QModelIndex();

    auto parent = m_childParentMap.value(entity);
    const auto parentIndex = indexForEntity(parent);
    if (!parentIndex.isValid() && parent)
        return QModelIndex();

    const auto &siblings = m_parentChildMap[parent];
    auto it = std::lower_bound(siblings.constBegin(), siblings.constEnd(), entity);
    if (it == siblings.constEnd() || *it != entity)
        return QModelIndex();

    const int row = std::distance(siblings.constBegin(), it);
    return index(row, 0, parentIndex);
}

static bool isEngineForEntity(Qt3DCore::QAspectEngine *engine, Qt3DCore::QEntity *entity)
{
    Q_ASSERT(engine);
    Q_ASSERT(entity);
    if (entity == engine->rootEntity())
        return true;
    if (!entity->parentEntity())
        return false;
    return isEngineForEntity(engine, entity->parentEntity());
}

void Qt3DEntityTreeModel::objectCreated(QObject *obj)
{
    if (!m_engine)
        return;

    auto entity = qobject_cast<Qt3DCore::QEntity *>(obj);
    if (!entity)
        return;

    if (!isEngineForEntity(m_engine, entity))
        return;

    if (m_childParentMap.contains(entity))
        return;

    auto parentEntity = entity->parentEntity();
    if (parentEntity) {
        // add parent first, if we don't know that yet
        if (!m_childParentMap.contains(parentEntity)) {
            objectCreated(parentEntity);
            return;
        }
    }

    const auto index = indexForEntity(parentEntity);
    Q_ASSERT(index.isValid() || !parentEntity);

    auto &children = m_parentChildMap[parentEntity];
    auto it = std::lower_bound(children.begin(), children.end(), entity);
    const int row = std::distance(children.begin(), it);

    beginInsertRows(index, row, row);
    children.insert(it, entity);
    m_childParentMap.insert(entity, parentEntity);
    connectEntity(entity);
    foreach (auto child, entity->childNodes()) {
        populateFromNode(child);
    }
    endInsertRows();
}

void Qt3DEntityTreeModel::objectDestroyed(QObject *obj)
{
    auto entity = static_cast<Qt3DCore::QEntity*>(obj); // never dereference this!
    if (!m_childParentMap.contains(entity)) {
        Q_ASSERT(!m_parentChildMap.contains(entity));
        return;
    }

    removeEntity(entity, true);
}

void Qt3DEntityTreeModel::removeEntity(Qt3DCore::QEntity *entity, bool danglingPointer)
{
    auto parentEntity = m_childParentMap.value(entity);
    const QModelIndex parentIndex = indexForEntity(parentEntity);
    if (parentEntity && !parentIndex.isValid())
        return;

    auto &siblings = m_parentChildMap[parentEntity];
    auto it = std::lower_bound(siblings.begin(), siblings.end(), entity);
    if (it == siblings.end() || *it != entity)
        return;
    const int row = std::distance(siblings.begin(), it);

    beginRemoveRows(parentIndex, row, row);
    siblings.erase(it);
    removeSubtree(entity, danglingPointer);
    endRemoveRows();
}

void Qt3DEntityTreeModel::removeSubtree(Qt3DCore::QEntity *entity, bool danglingPointer)
{
    if (!danglingPointer)
        disconnectEntity(entity);
    const auto children = m_parentChildMap.value(entity);
    for (auto child : children)
        removeSubtree(child, danglingPointer);
    m_childParentMap.remove(entity);
    m_parentChildMap.remove(entity);
}

void Qt3DEntityTreeModel::objectReparented(QObject *obj)
{
    auto entity = qobject_cast<Qt3DCore::QEntity *>(obj);
    if (!entity)
        return;

    if (m_childParentMap.contains(entity)) {
        // moved out of our tree
        if (!isEngineForEntity(m_engine, entity)) {
            removeEntity(entity, false);
        } else {
            // TODO reparented within our tree
        }
    } else {
        // possibly reparented into our tree
        objectCreated(obj);
    }
}

void Qt3DEntityTreeModel::connectEntity(Qt3DCore::QEntity *entity)
{
    connect(entity, &Qt3DCore::QEntity::enabledChanged, this, &Qt3DEntityTreeModel::entityEnabledChanged);
}

void Qt3DEntityTreeModel::disconnectEntity(Qt3DCore::QEntity *entity)
{
    disconnect(entity, &Qt3DCore::QEntity::enabledChanged, this, &Qt3DEntityTreeModel::entityEnabledChanged);
}

void Qt3DEntityTreeModel::entityEnabledChanged()
{
    auto entity = qobject_cast<Qt3DCore::QEntity*>(sender());
    if (!entity)
        return;
    const auto idx = indexForEntity(entity);
    if (!idx.isValid())
        return;
    emit dataChanged(idx, idx);
}
