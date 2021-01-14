/*
  framegraphmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <Qt3DRender/QRenderSettings>

#include <algorithm>

using namespace GammaRay;

FrameGraphModel::FrameGraphModel(QObject *parent)
    : ObjectModelBase<QAbstractItemModel>(parent)
    , m_settings(nullptr)
{
}

FrameGraphModel::~FrameGraphModel()
{
}

void FrameGraphModel::setRenderSettings(Qt3DRender::QRenderSettings *settings)
{
    beginResetModel();
    clear();
    m_settings = settings;
    // TODO monitor m_settings->activeFrameGraph changed
    if (m_settings)
        populateFromNode(m_settings->activeFrameGraph());
    endResetModel();
}

void FrameGraphModel::clear()
{
    for (auto it = m_childParentMap.constBegin(); it != m_childParentMap.constEnd(); ++it)
        disconnectNode(it.key());
    m_childParentMap.clear();
    m_parentChildMap.clear();
}

void FrameGraphModel::populateFromNode(Qt3DRender::QFrameGraphNode *node)
{
    if (!node)
        return;

    m_childParentMap[node] = node->parentFrameGraphNode();
    m_parentChildMap[node->parentFrameGraphNode()].push_back(node);
    connectNode(node);

    foreach (auto child, node->childNodes()) {
        if (auto childNode = qobject_cast<Qt3DRender::QFrameGraphNode *>(child))
            populateFromNode(childNode);
    }

    auto &children = m_parentChildMap[node->parentFrameGraphNode()];
    std::sort(children.begin(), children.end());
}

int FrameGraphModel::rowCount(const QModelIndex &parent) const
{
    auto parentNode = reinterpret_cast<Qt3DRender::QFrameGraphNode *>(parent.internalPointer());
    return m_parentChildMap.value(parentNode).size();
}

QVariant FrameGraphModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto node = reinterpret_cast<Qt3DRender::QFrameGraphNode *>(index.internalPointer());
    if (role == ObjectModel::ObjectIdRole)
        return QVariant::fromValue(ObjectId(node));
    else if (role == Qt::CheckStateRole && index.column() == 0)
        return node->isEnabled() ? Qt::Checked : Qt::Unchecked;

    return dataForObject(node, index, role);
}

QModelIndex FrameGraphModel::parent(const QModelIndex &child) const
{
    auto childNode = reinterpret_cast<Qt3DRender::QFrameGraphNode *>(child.internalPointer());
    return indexForNode(m_childParentMap.value(childNode));
}

QModelIndex FrameGraphModel::index(int row, int column, const QModelIndex &parent) const
{
    auto parentNode = reinterpret_cast<Qt3DRender::QFrameGraphNode *>(parent.internalPointer());
    const auto children = m_parentChildMap.value(parentNode);
    if (row < 0 || column < 0 || row >= children.size() || column >= columnCount())
        return QModelIndex();
    return createIndex(row, column, children.at(row));
}

Qt::ItemFlags FrameGraphModel::flags(const QModelIndex &index) const
{
    auto baseFlags = QAbstractItemModel::flags(index);
    if (index.isValid() && index.column() == 0)
        return baseFlags | Qt::ItemIsUserCheckable;
    return baseFlags;
}

bool FrameGraphModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!m_settings || !index.isValid() || role != Qt::CheckStateRole || index.column() != 0)
        return false;

    auto node = reinterpret_cast<Qt3DRender::QFrameGraphNode*>(index.internalPointer());
    node->setEnabled(value.toInt() == Qt::Checked);
    emit dataChanged(index, index);
    return true;
}

QModelIndex FrameGraphModel::indexForNode(Qt3DRender::QFrameGraphNode *node) const
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

static bool isRenderSettingsForNode(Qt3DRender::QRenderSettings *settings, Qt3DRender::QFrameGraphNode *node)
{
    Q_ASSERT(settings);
    Q_ASSERT(node);
    if (node == settings->activeFrameGraph())
        return true;
    if (!node->parentFrameGraphNode())
        return false;
    return isRenderSettingsForNode(settings, node->parentFrameGraphNode());
}

void FrameGraphModel::objectCreated(QObject *obj)
{
    auto node = qobject_cast<Qt3DRender::QFrameGraphNode*>(obj);
    if (!node || !m_settings)
        return;

    if (!isRenderSettingsForNode(m_settings, node))
        return;

    if (m_childParentMap.contains(node))
        return;

    auto parentNode = node->parentFrameGraphNode();
    if (parentNode) {
        // add parent first, if we don't know that yet
        if (!m_childParentMap.contains(parentNode)) {
            objectCreated(parentNode);
            return;
        }
    }

    const auto index = indexForNode(parentNode);
    Q_ASSERT(index.isValid() || !parentNode);

    auto &children = m_parentChildMap[parentNode];
    auto it = std::lower_bound(children.begin(), children.end(), node);
    const int row = std::distance(children.begin(), it);

    beginInsertRows(index, row, row);
    children.insert(it, node);
    m_childParentMap.insert(node, parentNode);
    connectNode(node);
    foreach (auto child, node->childNodes()) {
        if (auto childNode = qobject_cast<Qt3DRender::QFrameGraphNode *>(child))
            populateFromNode(childNode);
    }
    endInsertRows();
}

void FrameGraphModel::objectDestroyed(QObject *obj)
{
    auto node = static_cast<Qt3DRender::QFrameGraphNode*>(obj); // never dereference this!
    if (!m_childParentMap.contains(node)) {
        Q_ASSERT(!m_parentChildMap.contains(node));
        return;
    }

    removeNode(node, true);
}

void FrameGraphModel::removeNode(Qt3DRender::QFrameGraphNode *node, bool danglingPointer)
{
    if (!danglingPointer)
        disconnectNode(node);

    auto parentNode = m_childParentMap.value(node);
    const QModelIndex parentIndex = indexForNode(parentNode);
    if (parentNode && !parentIndex.isValid())
        return;

    auto &siblings = m_parentChildMap[parentNode];
    auto it = std::lower_bound(siblings.begin(), siblings.end(), node);
    if (it == siblings.end() || *it != node)
        return;
    const int row = std::distance(siblings.begin(), it);

    beginRemoveRows(parentIndex, row, row);
    siblings.erase(it);
    removeSubtree(node, danglingPointer);
    endRemoveRows();
}

void FrameGraphModel::removeSubtree(Qt3DRender::QFrameGraphNode *node, bool danglingPointer)
{
    const auto children = m_parentChildMap.value(node);
    for (auto child : children)
        removeSubtree(child, danglingPointer);
    m_childParentMap.remove(node);
    m_parentChildMap.remove(node);
}

void FrameGraphModel::objectReparented(QObject *obj)
{
    auto node = qobject_cast<Qt3DRender::QFrameGraphNode*>(obj);
    if (!node)
        return;

    if (m_childParentMap.contains(node)) {
        if (isRenderSettingsForNode(m_settings, node)) {
            // TODO reparented inside our tree
        } else {
            // moved to outside of our tree
            removeNode(node, false);
        }
    } else {
        // possibly reparented into our tree
        objectCreated(obj);
    }
}

void FrameGraphModel::connectNode(Qt3DRender::QFrameGraphNode *node)
{
    connect(node, &Qt3DRender::QFrameGraphNode::enabledChanged, this, &FrameGraphModel::nodeEnabledChanged);
}

void FrameGraphModel::disconnectNode(Qt3DRender::QFrameGraphNode *node)
{
    disconnect(node, &Qt3DRender::QFrameGraphNode::enabledChanged, this, &FrameGraphModel::nodeEnabledChanged);
}

void FrameGraphModel::nodeEnabledChanged()
{
    auto node = qobject_cast<Qt3DRender::QFrameGraphNode*>(sender());
    if (!node)
        return;
    const auto idx = indexForNode(node);
    if (!idx.isValid())
        return;
    emit dataChanged(idx, idx);
}
