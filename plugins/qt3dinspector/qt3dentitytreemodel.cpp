/*
  qt3dentitytreemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

Qt3DEntityTreeModel::Qt3DEntityTreeModel(QObject* parent):
    ObjectModelBase<QAbstractItemModel>(parent),
    m_engine(nullptr)
{
}

Qt3DEntityTreeModel::~Qt3DEntityTreeModel()
{
}

void Qt3DEntityTreeModel::setEngine(Qt3DCore::QAspectEngine* engine)
{
    beginResetModel();
    clear();
    m_engine = engine;
    populateFromEntity(engine->rootEntity().data());
    endResetModel();
}

void Qt3DEntityTreeModel::clear()
{
    m_childParentMap.clear();
    m_parentChildMap.clear();
}

void Qt3DEntityTreeModel::populateFromEntity(Qt3DCore::QEntity* entity)
{
    if (!entity)
        return;

    m_childParentMap[entity] = entity->parentEntity();
    m_parentChildMap[entity->parentEntity()].push_back(entity);

    foreach (auto child,  entity->childNodes()) {
        if (auto childEntity = qobject_cast<Qt3DCore::QEntity*>(child))
            populateFromEntity(childEntity);
    }

    auto &children  = m_parentChildMap[entity->parentEntity()];
    std::sort(children.begin(), children.end());
}

QVariant Qt3DEntityTreeModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || !m_engine)
        return QVariant();

    auto entity = reinterpret_cast<Qt3DCore::QEntity*>(index.internalPointer());
    if (role == ObjectModel::ObjectIdRole)
        return QVariant::fromValue(ObjectId(entity));

    return dataForObject(entity, index, role);
}

int Qt3DEntityTreeModel::rowCount(const QModelIndex& parent) const
{
    if (!m_engine || !m_engine->rootEntity())
        return 0;

    auto parentEntity = reinterpret_cast<Qt3DCore::QEntity*>(parent.internalPointer());
    return m_parentChildMap.value(parentEntity).size();
}

QModelIndex Qt3DEntityTreeModel::parent(const QModelIndex& child) const
{
    auto childEntity = reinterpret_cast<Qt3DCore::QEntity*>(child.internalPointer());
    return indexForEntity(m_childParentMap.value(childEntity));
}

QModelIndex Qt3DEntityTreeModel::index(int row, int column, const QModelIndex& parent) const
{
    auto parentEntity = reinterpret_cast<Qt3DCore::QEntity*>(parent.internalPointer());
    const auto children = m_parentChildMap.value(parentEntity);
    if (row < 0 || column < 0 || row >= children.size()  || column >= columnCount())
        return QModelIndex();
    return createIndex(row, column, children.at(row));
}

QModelIndex Qt3DEntityTreeModel::indexForEntity(Qt3DCore::QEntity* entity) const
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
