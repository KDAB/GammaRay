/*
  framegraphmodel.cpp

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

#include "framegraphmodel.h"

#include <Qt3DRender/QFrameGraphNode>

#include <algorithm>

using namespace GammaRay;

FrameGraphModel::FrameGraphModel(QObject* parent):
    ObjectModelBase<QAbstractItemModel>(parent)
{
}

FrameGraphModel::~FrameGraphModel()
{
}

void FrameGraphModel::setFrameGraph(Qt3DRender::QFrameGraphNode* frameGraph)
{
    beginResetModel();
    clear();
    populateFromNode(frameGraph);
    endResetModel();
}

void FrameGraphModel::clear()
{
    m_childParentMap.clear();
    m_parentChildMap.clear();
}

void FrameGraphModel::populateFromNode(Qt3DRender::QFrameGraphNode* node)
{
    if (!node)
        return;

    m_childParentMap[node] = node->parentFrameGraphNode();
    m_parentChildMap[node->parentFrameGraphNode()].push_back(node);

    foreach (auto child,  node->childNodes()) {
        if (auto childNode = qobject_cast<Qt3DRender::QFrameGraphNode*>(child))
            populateFromNode(childNode);
    }

    auto &children  = m_parentChildMap[node->parentFrameGraphNode()];
    std::sort(children.begin(), children.end());
}

int FrameGraphModel::rowCount(const QModelIndex& parent) const
{
    auto parentNode = reinterpret_cast<Qt3DRender::QFrameGraphNode*>(parent.internalPointer());
    return m_parentChildMap.value(parentNode).size();
}

QVariant FrameGraphModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto node = reinterpret_cast<Qt3DRender::QFrameGraphNode*>(index.internalPointer());
    if (role == ObjectModel::ObjectIdRole)
        return QVariant::fromValue(ObjectId(node));

    return dataForObject(node, index, role);
}

QModelIndex FrameGraphModel::parent(const QModelIndex& child) const
{
    auto childNode = reinterpret_cast<Qt3DRender::QFrameGraphNode*>(child.internalPointer());
    return indexForNode(m_childParentMap.value(childNode));
}

QModelIndex FrameGraphModel::index(int row, int column, const QModelIndex& parent) const
{
    auto parentNode = reinterpret_cast<Qt3DRender::QFrameGraphNode*>(parent.internalPointer());
    const auto children = m_parentChildMap.value(parentNode);
    if (row < 0 || column < 0 || row >= children.size()  || column >= columnCount())
        return QModelIndex();
    return createIndex(row, column, children.at(row));
}

QModelIndex FrameGraphModel::indexForNode(Qt3DRender::QFrameGraphNode* node) const
{
    if (!node)
        return QModelIndex();

    auto parent = m_childParentMap.value(node);
    const auto parentIndex = indexForNode(parent);
    if (!parentIndex.isValid() && parent)
        return QModelIndex();

    const auto &siblings = m_parentChildMap[parent];
    auto it = std::lower_bound(siblings.constBegin(), siblings.constEnd(), node);
    if (it == siblings.constEnd() || *it != node)
        return QModelIndex();

    const int row = std::distance(siblings.constBegin(), it);
    return index(row, 0, parentIndex);
}
