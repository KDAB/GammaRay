/*
  quickscenegraphmodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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

template<typename Container, typename Value>
static bool contains(const Container &c, const Value &v)
{
    return c.find(v) != c.cend();
}

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
        connect(m_window.data(), &QQuickWindow::afterRendering, this, [this] { updateSGTree(); });
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
        m_itemNodesDirty = true;
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
        }
        if (index.column() == 1) {
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

    auto it = m_parentChildMap.find(parentNode);
    if (it != m_parentChildMap.end()) {
        return ( int )it->second.size();
    }
    return 0;
}

QModelIndex QuickSceneGraphModel::parent(const QModelIndex &child) const
{
    QSGNode *childNode = reinterpret_cast<QSGNode *>(child.internalPointer());

    auto it = m_childParentMap.find(childNode);
    if (it != m_childParentMap.end()) {
        return indexForNode(it->second);
    }
    return {};
}

QModelIndex QuickSceneGraphModel::index(int row, int column, const QModelIndex &parent) const
{
    QSGNode *parentNode = reinterpret_cast<QSGNode *>(parent.internalPointer());

    auto it = m_parentChildMap.find(parentNode);
    if (it == m_parentChildMap.end()) {
        return {};
    }

    const QVector<QSGNode *> &children = it->second;

    if (row < 0 || column < 0 || row >= children.size() || column >= columnCount())
        return {};

    return createIndex(row, column, children.at(row));
}

void QuickSceneGraphModel::clear()
{
    m_childParentMap.clear();
    m_parentChildMap.clear();
    m_itemNodesDirty = true;
}

// indexForNode() is expensive, so only use it when really needed
#define GET_INDEX                     \
    if (emitSignals && !hasMyIndex) { \
        myIndex = indexForNode(node); \
        hasMyIndex = true;            \
    }

static QVector<QSGNode *> sortedChildren(QSGNode *node)
{
    QVector<QSGNode *> children;
    for (QSGNode *childNode = node->firstChild(); childNode; childNode = childNode->nextSibling())
        children.append(childNode);
    std::sort(children.begin(), children.end());
    return children;
}

// childList is sorted, and a node can't be in the list of children twice
static bool hasSameChildren(QSGNode *node, const QVector<QSGNode *> &childList)
{
    int count = 0;
    for (QSGNode *childNode = node->firstChild(); childNode; childNode = childNode->nextSibling()) {
        if (count == childList.size() || !std::binary_search(childList.cbegin(), childList.cend(), childNode))
            return false;
        ++count;
    }
    return count == childList.size();
}

void QuickSceneGraphModel::populateFromNode(QSGNode *node, bool emitSignals)
{
    if (!node)
        return;

    QVector<QSGNode *> &childList = m_parentChildMap[node];

    // nothing changed in the common case, check that without allocating anything
    if (hasSameChildren(node, childList)) {
        for (int row = 0; row < childList.size(); ++row)
            populateFromNode(childList.at(row), emitSignals);
        return;
    }

    const QVector<QSGNode *> newChildList = sortedChildren(node);

    QModelIndex myIndex; // don't call indexForNode(node) here yet, in the common case of few changes we waste a lot of time here
    bool hasMyIndex = false;

    // Remove the children that are gone, either deleted or moved elsewhere. We can't tell these
    // apart, as a deleted node must not be touched anymore. Moved nodes are added again where we
    // find them. Doing this first ensures that the lists of our ancestors only contain their
    // actual children, so nothing below modifies a list that is iterated further up.
    for (int last = childList.size() - 1; last >= 0;) {
        if (std::binary_search(newChildList.cbegin(), newChildList.cend(), childList.at(last))) {
            --last;
            continue;
        }
        int first = last;
        while (first > 0 && !std::binary_search(newChildList.cbegin(), newChildList.cend(), childList.at(first - 1)))
            --first;

        for (int row = first; row <= last; ++row)
            emit nodeDeleted(childList.at(row));
        GET_INDEX
        if (emitSignals)
            beginRemoveRows(myIndex, first, last);
        for (int row = first; row <= last; ++row)
            pruneSubTree(childList.at(row));
        childList.remove(first, last - first + 1);
        if (emitSignals)
            endRemoveRows();
        last = first - 1;
    }

    // childList is a subset of newChildList now, insert the missing ones in blocks of consecutive rows
    int row = 0;
    for (int j = 0; j < newChildList.size();) {
        if (row < childList.size() && childList.at(row) == newChildList.at(j)) {
            populateFromNode(newChildList.at(j), emitSignals);
            ++row;
            ++j;
            continue;
        }
        int end = j;
        while (end < newChildList.size() && (row >= childList.size() || childList.at(row) != newChildList.at(end)))
            ++end;

        // build the new subtrees completely before announcing them, they are unreachable through
        // the model until they are in childList
        for (int k = j; k < end; ++k)
            addSubTree(newChildList.at(k), emitSignals);

        GET_INDEX
        if (emitSignals)
            beginInsertRows(myIndex, row, row + end - j - 1);
        for (int k = j; k < end; ++k)
            m_childParentMap[newChildList.at(k)] = node;
        childList.insert(row, end - j, nullptr);
        std::copy(newChildList.cbegin() + j, newChildList.cbegin() + end, childList.begin() + row);
        if (emitSignals)
            endInsertRows();
        row += end - j;
        j = end;
    }

    Q_ASSERT(childList == newChildList);
}

#undef GET_INDEX

void QuickSceneGraphModel::addSubTree(QSGNode *node, bool emitSignals)
{
    // we still know this node from its previous position, which hasn't been processed yet
    if (contains(m_childParentMap, node))
        removeNode(node, emitSignals);

    const QVector<QSGNode *> children = sortedChildren(node);
    for (QSGNode *child : children) {
        addSubTree(child, emitSignals);
        m_childParentMap[child] = node;
    }
    m_parentChildMap[node] = children;
}

void QuickSceneGraphModel::removeNode(QSGNode *node, bool emitSignals)
{
    const QModelIndex index = indexForNode(node);
    Q_ASSERT(index.isValid());
    QSGNode *parentNode = m_childParentMap.at(node);

    if (emitSignals)
        beginRemoveRows(index.parent(), index.row(), index.row());
    pruneSubTree(node);
    m_parentChildMap[parentNode].remove(index.row());
    if (emitSignals)
        endRemoveRows();
}

// only needed when looking up items, so not done after every frame
void QuickSceneGraphModel::updateItemNodes() const
{
    if (!m_itemNodesDirty)
        return;
    m_itemNodesDirty = false;
    m_itemItemNodeMap.clear();
    m_itemNodeItemMap.clear();
    if (m_window)
        collectItemNodes(m_window->contentItem());
}

void QuickSceneGraphModel::collectItemNodes(QQuickItem *item) const
{
    if (!item)
        return;

    QQuickItemPrivate *priv = QQuickItemPrivate::get(item);
    if (!priv->itemNodeInstance) // Explicitly avoid calling priv->itemNode() here, which would create a new node outside the scenegraph's behavior.
        return;

    QSGNode *itemNode = priv->itemNodeInstance;
    m_itemItemNodeMap[item] = itemNode;
    m_itemNodeItemMap[itemNode] = item;

    for (QQuickItem *child : std::as_const(priv->childItems))
        collectItemNodes(child);
}

QModelIndex QuickSceneGraphModel::indexForNode(QSGNode *node) const
{
    if (!node)
        return {};

    auto cit = m_childParentMap.find(node);
    QSGNode *parent = nullptr;
    if (cit != m_childParentMap.end()) {
        parent = cit->second;
    }

    auto pit = m_parentChildMap.find(parent);
    if (pit == m_parentChildMap.end()) {
        return {};
    }

    const QVector<QSGNode *> &siblings = pit->second;
    auto it = std::lower_bound(siblings.constBegin(), siblings.constEnd(), node);
    if (it == siblings.constEnd() || *it != node)
        return QModelIndex();

    const int row = std::distance(siblings.constBegin(), it);
    return createIndex(row, 0, node);
}

QSGNode *QuickSceneGraphModel::sgNodeForItem(QQuickItem *item) const
{
    updateItemNodes();
    auto it = m_itemItemNodeMap.find(item);
    if (it != m_itemItemNodeMap.end()) {
        return it->second;
    }
    return nullptr;
}

QQuickItem *QuickSceneGraphModel::itemForSgNode(QSGNode *node) const
{
    updateItemNodes();
    while (node && !contains(m_itemNodeItemMap, node)) {
        // If there's no entry for node, take its parent
        auto it = m_childParentMap.find(node);
        if (it == m_childParentMap.end()) {
            // Can happen if the node belongs to another window and itemForSgNode() gets
            // called during window selection change
            return nullptr;
        } else {
            node = it->second;
        }
    }

    auto it = m_itemNodeItemMap.find(node);
    if (it != m_itemNodeItemMap.end()) {
        return it->second;
    }
    return nullptr;
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
    auto it = m_parentChildMap.find(node);
    if (it != m_parentChildMap.end()) {
        const QVector<QSGNode *> children = it->second;
        for (auto child : children)
            pruneSubTree(child);
        m_parentChildMap.erase(node);
    }
    m_childParentMap.erase(node);
}
