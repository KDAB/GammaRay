/*
  quickscenegraphmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#include "quickscenegraphmodel.h"

#include <private/qquickitem_p.h>
#include "quickitemmodelroles.h"

#include <QQuickWindow>
#include <QThread>
#include <QSGNode>

#include <algorithm>

Q_DECLARE_METATYPE(QSGNode *)

using namespace GammaRay;

QuickSceneGraphModel::QuickSceneGraphModel(QObject *parent)
    : ObjectModelBase<QAbstractItemModel>(parent)
    , m_rootNode(nullptr)
{
}

QuickSceneGraphModel::~QuickSceneGraphModel() = default;

void QuickSceneGraphModel::setWindow(QQuickWindow *window)
{
    beginResetModel();
    clear();
    if (m_window)
        disconnect(m_window.data(), &QQuickWindow::afterRendering, this, nullptr);
    m_window = window;
    m_rootNode = currentRootNode();
    if (m_window && m_rootNode) {
        updateSGTree(false);
        connect(m_window.data(), &QQuickWindow::afterRendering, this, [this]{ updateSGTree(); });
    }

    endResetModel();
}

void QuickSceneGraphModel::updateSGTree(bool emitSignals)
{
    auto root = currentRootNode();
    if (root != m_rootNode) { // everything changed, reset
        beginResetModel();
        clear();
        m_rootNode = root;
        if (m_window && m_rootNode)
            updateSGTree(false);
        endResetModel();
    } else {
        m_childParentMap[m_rootNode] = nullptr;
        m_parentChildMap[nullptr].resize(1);
        m_parentChildMap[nullptr][0] = m_rootNode;

        populateFromNode(m_rootNode, emitSignals);
        collectItemNodes(m_window->contentItem());
    }
}

QSGNode *QuickSceneGraphModel::currentRootNode() const
{
    if (!m_window)
        return nullptr;

    QQuickItem *item = m_window->contentItem();
    QQuickItemPrivate *itemPriv = QQuickItemPrivate::get(item);
    QSGNode *root = itemPriv->itemNode();
    while (root->parent()) // Ensure that we really get the very root node.
        root = root->parent();
    return root;
}

QVariant QuickSceneGraphModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    QSGNode *node = reinterpret_cast<QSGNode *>(index.internalPointer());

    if (role == Qt::DisplayRole) {
        if (index.column() == 0) {
            return Util::addressToString(node);
        } else if (index.column() == 1) {
            switch (node->type()) {
            case QSGNode::BasicNodeType:
                return "Node";
            case QSGNode::GeometryNodeType:
                return "Geometry Node";
            case QSGNode::TransformNodeType:
                return "Transform Node";
            case QSGNode::ClipNodeType:
                return "Clip Node";
            case QSGNode::OpacityNodeType:
                return "Opacity Node";
            case QSGNode::RootNodeType:
                return "Root Node";
            case QSGNode::RenderNodeType:
                return "Render Node";
            }
        }
    } else if (role == ObjectModel::ObjectRole) {
        return QVariant::fromValue(node);
    }

    return QVariant();
}

int QuickSceneGraphModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() == 1)
        return 0;

    QSGNode *parentNode = reinterpret_cast<QSGNode *>(parent.internalPointer());
    return m_parentChildMap.value(parentNode).size();
}

QModelIndex QuickSceneGraphModel::parent(const QModelIndex &child) const
{
    QSGNode *childNode = reinterpret_cast<QSGNode *>(child.internalPointer());
    return indexForNode(m_childParentMap.value(childNode));
}

QModelIndex QuickSceneGraphModel::index(int row, int column, const QModelIndex &parent) const
{
    QSGNode *parentNode = reinterpret_cast<QSGNode *>(parent.internalPointer());
    const QVector<QSGNode *> children = m_parentChildMap.value(parentNode);

    if (row < 0 || column < 0 || row >= children.size() || column >= columnCount())
        return {};

    return createIndex(row, column, children.at(row));
}

void QuickSceneGraphModel::clear()
{
    m_childParentMap.clear();
    m_parentChildMap.clear();
}

// indexForNode() is expensive, so only use it when really needed
#define GET_INDEX if (emitSignals && !hasMyIndex) { myIndex = indexForNode(node); hasMyIndex = true; \
}

void QuickSceneGraphModel::populateFromNode(QSGNode *node, bool emitSignals)
{
    if (!node)
        return;

    QVector<QSGNode *> &childList = m_parentChildMap[node];
    QVector<QSGNode *> newChildList;

    newChildList.reserve(node->childCount());
    for (QSGNode *childNode = node->firstChild(); childNode; childNode = childNode->nextSibling())
        newChildList.append(childNode);

    QModelIndex myIndex; // don't call indexForNode(node) here yet, in the common case of few changes we waste a lot of time here
    bool hasMyIndex = false;

    std::sort(newChildList.begin(), newChildList.end());

    auto i = childList.begin();
    auto j = newChildList.constBegin();

    while (i != childList.end() && j != newChildList.constEnd()) {
        if (*i < *j) { // handle deleted node
            emit nodeDeleted(*i);
            GET_INDEX
            if (emitSignals) {
                const auto idx = std::distance(childList.begin(), i);
                beginRemoveRows(myIndex, idx, idx);
            }
            pruneSubTree(*i);
            i = childList.erase(i);
            if (emitSignals)
                endRemoveRows();
        } else if (*i > *j) { // handle added node
            GET_INDEX
            const auto idx = std::distance(childList.begin(), i);
            if (m_childParentMap.contains(*j)) { // move from elsewhere in our tree
                const auto sourceIdx = indexForNode(*j);
                Q_ASSERT(sourceIdx.isValid());
#if 0
                if (emitSignals)
                    beginMoveRows(sourceIdx.parent(), sourceIdx.row(), sourceIdx.row(), myIndex,
                                  idx);
                m_parentChildMap[m_childParentMap.value(*j)].remove(sourceIdx.row());
                m_childParentMap.insert(*j, node);
                i = childList.insert(i, *j);
                if (emitSignals)
                    endMoveRows();
#else
                if (emitSignals) {
                    beginRemoveRows(sourceIdx.parent(), sourceIdx.row(), sourceIdx.row());
                }
                m_parentChildMap[m_childParentMap.value(*j)].remove(sourceIdx.row());
                m_childParentMap.remove(*j);
                if (emitSignals) {
                    endRemoveRows();
                    beginInsertRows(myIndex, idx, idx);
                }
                m_childParentMap.insert(*j, node);
                i = childList.insert(i, *j);
                if (emitSignals) {
                    endInsertRows();
                }
#endif
                populateFromNode(*j, emitSignals);
            } else { // entirely new
                if (emitSignals)
                    beginInsertRows(myIndex, idx, idx);
                m_childParentMap.insert(*j, node);
                i = childList.insert(i, *j);
                populateFromNode(*j, false);
                if (emitSignals)
                    endInsertRows();
            }
            ++i;
            ++j;
        } else { // already known node, no change
            populateFromNode(*j, emitSignals);
            ++i;
            ++j;
        }
    }
    if (i == childList.end() && j != newChildList.constEnd()) {
        // Add remaining new items to list and inform the client
        // process the remaining items in pairs of n entirely new ones and 0-1 moved ones
        GET_INDEX
        while (j != newChildList.constEnd()) {
            const auto newBegin = j;
            while (j != newChildList.constEnd() && !m_childParentMap.contains(*j))
                ++j;

            // newBegin to j - 1 is new, j is either moved or end
            if (newBegin != j) { // new elements
                if (emitSignals) {
                    const auto idx = childList.size();
                    const auto count = std::distance(newBegin, j);
                    beginInsertRows(myIndex, idx, idx + count - 1);
                }
                for (auto it = newBegin; it != j; ++it) {
                    m_childParentMap.insert(*it, node);
                    childList.append(*it);
                }
                for (auto it = newBegin; it != j; ++it)
                    populateFromNode(*it, false);
                if (emitSignals)
                    endInsertRows();
            }

            if (j != newChildList.constEnd() && m_childParentMap.contains(*j)) { // one moved element, important to recheck if this is still a move, in case the above has removed it meanwhile...
                const auto sourceIdx = indexForNode(*j);
                Q_ASSERT(sourceIdx.isValid());
#if 0
                if (emitSignals) {
                    const auto idx = childList.size();
                    beginMoveRows(sourceIdx.parent(), sourceIdx.row(), sourceIdx.row(), myIndex,
                                  idx);
                }
                m_parentChildMap[m_childParentMap.value(*j)].remove(sourceIdx.row());
                m_childParentMap.insert(*j, node);
                childList.append(*j);
                if (emitSignals)
                    endMoveRows();
#else
                if (emitSignals) {
                    beginRemoveRows(sourceIdx.parent(), sourceIdx.row(), sourceIdx.row());
                }
                m_parentChildMap[m_childParentMap.value(*j)].remove(sourceIdx.row());
                m_childParentMap.remove(*j);
                if (emitSignals) {
                    endRemoveRows();
                    const auto idx = childList.size();
                    beginInsertRows(myIndex, idx, idx);
                }
                m_childParentMap.insert(*j, node);
                childList.append(*j);
                if (emitSignals) {
                    endInsertRows();
                }
#endif
                populateFromNode(*j, emitSignals);
                ++j;
            }
        }
    } else if (i != childList.end()) { // Inform the client about the removed rows
        GET_INDEX
        const auto idx = std::distance(childList.begin(), i);
        const auto count = std::distance(i, childList.end());

        for (auto it = i; it != childList.end(); ++it)
            emit nodeDeleted(*it);

        if (emitSignals)
            beginRemoveRows(myIndex, idx, idx + count - 1);
        for (; i != childList.end(); ++i)
            pruneSubTree(*i);
        childList.remove(idx, count);
        if (emitSignals)
            endRemoveRows();
    }

    Q_ASSERT(childList == newChildList);
}

#undef GET_INDEX

void QuickSceneGraphModel::collectItemNodes(QQuickItem *item)
{
    if (!item)
        return;

    QQuickItemPrivate *priv = QQuickItemPrivate::get(item);
    if (!priv->itemNodeInstance) // Explicitly avoid calling priv->itemNode() here, which would create a new node outside the scenegraph's behavior.
        return;

    QSGNode *itemNode = priv->itemNodeInstance;
    m_itemItemNodeMap[item] = itemNode;
    m_itemNodeItemMap[itemNode] = item;

    foreach (QQuickItem *child, item->childItems())
        collectItemNodes(child);
}

QModelIndex QuickSceneGraphModel::indexForNode(QSGNode *node) const
{
    if (!node)
        return {};

    QSGNode *parent = m_childParentMap.value(node);

    const QVector<QSGNode *> &siblings = m_parentChildMap[parent];
    auto it = std::lower_bound(siblings.constBegin(), siblings.constEnd(), node);
    if (it == siblings.constEnd() || *it != node)
        return QModelIndex();

    const int row = std::distance(siblings.constBegin(), it);
    return createIndex(row, 0, node);
}

QSGNode *QuickSceneGraphModel::sgNodeForItem(QQuickItem *item) const
{
    return m_itemItemNodeMap[item];
}

QQuickItem *QuickSceneGraphModel::itemForSgNode(QSGNode *node) const
{
    while (node && !m_itemNodeItemMap.contains(node)) {
        // If there's no entry for node, take its parent
        node = m_childParentMap[node];
    }
    return m_itemNodeItemMap[node];
}

bool QuickSceneGraphModel::verifyNodeValidity(QSGNode *node)
{
    if (node == m_rootNode)
        return true;

    QQuickItem *item = itemForSgNode(node);
    QSGNode *itemNode = QQuickItemPrivate::get(item)->itemNode();
    bool valid = itemNode == node || recursivelyFindChild(itemNode, node);
    if (!valid) {
        // The tree got dirty without us noticing. Expecting more to be invalid,
        // so update the whole tree to ensure it's current.
        setWindow(m_window);
    }
    return valid;
}

bool QuickSceneGraphModel::recursivelyFindChild(QSGNode *root, QSGNode *child) const
{
    for (QSGNode *childNode = root->firstChild(); childNode; childNode = childNode->nextSibling()) {
        if (childNode == child || recursivelyFindChild(childNode, child))
            return true;
    }
    return false;
}

void QuickSceneGraphModel::pruneSubTree(QSGNode *node)
{
    foreach (auto child, m_parentChildMap.value(node))
        pruneSubTree(child);
    m_parentChildMap.remove(node);
    m_childParentMap.remove(node);
}
