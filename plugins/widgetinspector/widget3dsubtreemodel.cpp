/*
  widget3dsubtreemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

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

#include "widget3dsubtreemodel.h"

using namespace GammaRay;

class Widget3DSubtreeModel::Node
{
public:
    Node(const QModelIndex &idx)
        : sourceIdx(idx)
    {}

    int realChildrenCount() const
    {
        int count = 0;
        for (int i = 0, c = children.count(); i < c; ++i) {
            if (children[i]) {
                count += 1 + children[i]->realChildrenCount();
            }
        }
        return count;
    }

    QPersistentModelIndex sourceIdx;
    Node *parent;
    QList<Node*> children;
};

Widget3DSubtreeModel::Widget3DSubtreeModel(QObject *parent)
    : QAbstractProxyModel(parent)
{
}

Widget3DSubtreeModel::~Widget3DSubtreeModel()
{
    qDeleteAll(mNodeList);
    mNodeList.clear();
    mNodeLookup.clear();
}

void Widget3DSubtreeModel::setSourceModel(QAbstractItemModel *newSource)
{
    if (sourceModel()) {
        disconnect(sourceModel(), Q_NULLPTR, this, Q_NULLPTR);
    }

    QAbstractProxyModel::setSourceModel(newSource);
    connect(newSource, &QAbstractItemModel::rowsInserted,
            this, &Widget3DSubtreeModel::sourceRowsInserted);
    connect(newSource, &QAbstractItemModel::rowsAboutToBeRemoved,
            this, &Widget3DSubtreeModel::sourceRowsAboutToBeRemoved);
    connect(newSource, &QAbstractItemModel::dataChanged,
            this, &Widget3DSubtreeModel::sourceDataChanged);
    connect(newSource, &QAbstractItemModel::modelReset,
            this, &Widget3DSubtreeModel::sourceModelReset);
    connect(newSource, &QAbstractItemModel::layoutChanged,
            this, &Widget3DSubtreeModel::sourceLayoutChanged);

    resetModel();
}

void Widget3DSubtreeModel::setRootObjectId(const QString &rootObject)
{
    if (m_rootObject == rootObject) {
        return;
    }

    m_rootObject = rootObject;
    m_rootIndex = findIndexForObject(rootObject);
    Q_EMIT rootObjectIdChanged();
    resetModel();
}

QString Widget3DSubtreeModel::rootObjectId() const
{
    return m_rootObject;
}

void Widget3DSubtreeModel::resetModel()
{
    beginResetModel();
    m_foreignWindows.clear();
    qDeleteAll(mNodeList);
    mNodeList.clear();
    mNodeLookup.clear();
    if (!m_rootObject.isEmpty()) {
        populate();
    }
    endResetModel();
}

void Widget3DSubtreeModel::populate()
{
    if (!sourceModel()) {
        return;
    }

    QList<QModelIndex> toVisit = { m_rootIndex };
    while (!toVisit.isEmpty()) {
        const auto index = toVisit.takeFirst();
        Node *parent = Q_NULLPTR;
        if (index != m_rootIndex) {
            parent = mNodeLookup.value(index.parent().data(Widget3DModel::IdRole).toString());
        }
        if (index.data(Widget3DModel::IdRole).toString().isEmpty()) {
            continue;
        }
        if (index.data(Widget3DModel::IsWindowRole).toBool()) {
            m_foreignWindows.insert(index);
            if (index != m_rootIndex) {
                if (parent) {
                    parent->children.push_back(Q_NULLPTR);
                }
                continue;
            }
        }

        Node *node = new Node(index);
        node->parent = parent;
        if (parent) {
            parent->children.push_back(node);
        }
        const int idx = parent ? mNodeList.indexOf(parent) + 1 : mNodeList.size();
        Q_ASSERT(idx > (parent ? 0 : -1));
        mNodeList.insert(idx, node);
        mNodeLookup.insert(index.data(Widget3DModel::IdRole).toString(), node);

        for (int i = 0, c = sourceModel()->rowCount(index); i < c; ++i) {
            const auto child = index.child(i, 0);
            Q_ASSERT(child.isValid());
            toVisit.push_front(child);
        }
    }
}

QModelIndex Widget3DSubtreeModel::findIndexForObject(const QString &objectId) const
{
    if (!sourceModel()) {
        return QModelIndex();
    }

    QList<QModelIndex> toVisit = { QModelIndex() };
    while (!toVisit.isEmpty()) {
        const auto idx = toVisit.takeFirst();
        const QString v = sourceModel()->data(idx, Widget3DModel::IdRole).toString();
        if (v == objectId) {
            return idx;
        }

        for (int i = 0, c = sourceModel()->rowCount(idx); i < c; ++i) {
            toVisit.prepend(sourceModel()->index(i, 0, idx));
        }
    }

    return QModelIndex();
}

bool Widget3DSubtreeModel::isParentOf(const QModelIndex &parent, const QModelIndex &child) const
{
    QModelIndex c = child;
    while (c.isValid()) {
        if (c == parent) {
            return true;
        }
        if (m_foreignWindows.contains(c)) {
            return false;
        }
        if (c.data(Widget3DModel::IsWindowRole).toBool()) {
            m_foreignWindows.insert(c);
            return false;
        }

        c = c.parent();
    }
    return false;
}

bool Widget3DSubtreeModel::belongsToModel(const QModelIndex &idx) const
{
    if (!m_rootIndex.isValid()) {
        return true;
    }

    return isParentOf(m_rootIndex, idx);
}

int Widget3DSubtreeModel::sourceRowsInserted(const QModelIndex &sourceParent, int first, int last)
{
    Q_ASSERT(sourceModel());

    Q_ASSERT(!sourceParent.isValid() || !sourceParent.data(Widget3DModel::IdRole).toString().isEmpty());
    Node *parentNode = sourceParent.isValid() ? mNodeLookup.value(sourceParent.data(Widget3DModel::IdRole).toString()) : Q_NULLPTR;

    // Not for our current window
    if (sourceParent.isValid() && !parentNode) {
        return 0;
    }

    // Get index of parent
    const int parentIdx = parentNode ? mNodeList.indexOf(parentNode) : mNodeList.size() - 1;
    Q_ASSERT(parentIdx > (parentNode ? -1 : -2));

    // Recursively count all descendants of "parent" between 0 and "first"
    int realSiblings = 0;
    Q_ASSERT(!parentNode || first <= parentNode->children.count());
    for (int i = 0; parentNode && i < first; ++i) {
        if (Node *node = parentNode->children.at(i)) {
            realSiblings += 1 + node->realChildrenCount();
        }
    }

    // This is the position where we insert the first item. It includes all
    // descendants of our "above" siblings.
    int insertIndex = parentIdx + realSiblings + 1;
    for (int i = first; i <= last; ++i) {
        const QModelIndex idx = sourceModel()->index(i, 0, sourceParent);
        Q_ASSERT(idx.isValid());
        // Is window? Skip...
        if (idx.data(Widget3DModel::IsWindowRole).toBool()) {
            m_foreignWindows.insert(idx);
            if (parentNode) {
                parentNode->children.insert(i, Q_NULLPTR);
            }
            continue;
        }

        const auto idRole = idx.data(Widget3DModel::IdRole).toString();
        if (mNodeLookup.contains(idRole)) {
            // Could happen if populate() is "too fast"
            qWarning() << "Insert for object I already know!";
        } else {
            Q_ASSERT(insertIndex > parentIdx);
            beginInsertRows(QModelIndex(), insertIndex, insertIndex);
            Node *node = new Node(idx);
            node->parent = parentNode;
            if (parentNode) {
                parentNode->children.insert(i, node);
            }
            mNodeList.insert(insertIndex, node);
            mNodeLookup.insert(idRole, node);
            endInsertRows();
            ++insertIndex;
        }

        // Recursively check if the newly inserted row may have descendants.
        // When a subtree becomes visible we sometimes only get insert notification
        // for the root of the subtree, but not for the descendants
        const int childCnt = sourceModel()->rowCount(idx);
        if (childCnt) {
            insertIndex += sourceRowsInserted(idx, 0, childCnt - 1);
        }
    }
    if (parentNode) {
        Q_ASSERT(parentNode->children.count() == sourceModel()->rowCount(sourceParent));
    }

    return insertIndex - parentIdx - realSiblings;
}

void GammaRay::Widget3DSubtreeModel::sourceRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    Q_ASSERT(sourceModel());

    // Check if a parent of our root index is being removed
    QModelIndex idx = m_rootIndex.parent();
    while (idx.isValid()) {
        if (idx == parent) {
            resetModel();
            return;
        }
        idx = idx.parent();
    }

    // Did not happen within our subtree
    Node *parentNode = mNodeLookup.value(parent.data(Widget3DModel::IdRole).toString());
    if (!parentNode) {
        return;
    }

    for (int i = last; i >= first; --i) {
        QModelIndex idx = parent.child(i, 0);

        // Find the node that we are removing and its position in mNodeList
        if (!parentNode->children.at(i)) {
            Q_ASSERT(!mNodeLookup.contains(idx.data(Widget3DModel::IdRole).toString()));
            parentNode->children.removeAt(i);
            continue;
        }
        const int beginPos = mNodeList.indexOf(parentNode->children.at(i));
        Q_ASSERT(beginPos > -1);


        // TODO: Don't use indexes and has lookup, instead look at node->children.at(i + 1)
        // and check if it's null or not. If "i + 1" is out of bounds, than go up one
        // level etc.

        // Find idx's sibling, or its parent's sibling, or its parent's parent's
        // sibling etc. Everything between beginPos and the sibling are descendants
        // of the removed item and must be removed too.
        int endPos = -1;
        auto lookupIt = mNodeLookup.constEnd();
        QModelIndex next = idx.sibling(idx.row() + 1, 0);
        while (lookupIt == mNodeLookup.constEnd() && idx.isValid()) {
            lookupIt = mNodeLookup.constFind(next.data(Widget3DModel::IdRole).toString());
            next = idx.sibling(idx.row() + 1, 0);
            while (!next.isValid()) {
                const QModelIndex p = idx.parent();
                if (!p.isValid()) {
                    break;
                }
                next = p.sibling(p.row() + 1, 0);
                idx = p;
            }
            idx = next;
        }
        if (lookupIt == mNodeLookup.constEnd()) {
            endPos = mNodeList.size() - 1;
        } else {
            endPos = mNodeList.indexOf(*lookupIt) - 1;
        }
        Q_ASSERT(endPos >= beginPos);

        beginRemoveRows(QModelIndex(), beginPos, endPos);
        for (int j = endPos; j >= beginPos; --j) {
            auto node = mNodeList.takeAt(j);
            if (Node *p = node->parent) {
                p->children.removeOne(node);
            }

            // Expensive assert that makes sure that we never remove anything
            // that is not a descendant of "parent"
            Q_ASSERT([parent](QModelIndex si) {
                        while (si.isValid()) {
                            if (si == parent) {
                                return si;
                            }
                            si = si.parent();
                        }
                        return si;
                    }(node->sourceIdx) == parent);

            mNodeLookup.remove(node->sourceIdx.data(Widget3DModel::IdRole).toString());
            delete node;
        }
        endRemoveRows();
    }
    // TODO: Maybe rowCount() is not reliable at this point? Maybe compare in
    // sourceRowsRemoved()
    Q_ASSERT(parentNode->children.count() == sourceModel()->rowCount(parent) - (last - first + 1));
}

void Widget3DSubtreeModel::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_ASSERT(sourceModel());

    for (int i = topLeft.row(); i < bottomRight.row(); ++i) {
        const QModelIndex idx = topLeft.sibling(i, 0);
        Node *node = mNodeLookup.value(idx.data(Widget3DModel::IdRole).toString());
        if (!node) {
            continue;
        }
        const QModelIndex nodeIdx = indexForNode(node);
        Q_EMIT dataChanged(nodeIdx, nodeIdx);
    }
}

void Widget3DSubtreeModel::sourceLayoutChanged()
{
    resetModel();
}

void Widget3DSubtreeModel::sourceModelReset()
{
    resetModel();
}

int Widget3DSubtreeModel::columnCount(const QModelIndex &) const
{
    return 1;
}

int Widget3DSubtreeModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : mNodeList.count();
}

QModelIndex Widget3DSubtreeModel::index(int row, int column, const QModelIndex &parent) const
{
    auto parentNode = static_cast<Node*>(parent.internalPointer());

    const int parentPos = parentNode ? mNodeList.indexOf(parentNode) : 0;
    Q_ASSERT(parentPos > -1);

    const int pos = parentPos + row;
    if (row < 0 || pos >= mNodeList.size() || column != 0) {
        return QModelIndex();
    }

    return createIndex(pos, column, mNodeList.at(pos));
}

QModelIndex Widget3DSubtreeModel::parent(const QModelIndex &) const
{
    return QModelIndex(); // flat list
}

bool Widget3DSubtreeModel::hasChildren(const QModelIndex &parent) const
{
    return !parent.isValid();
}


QVariant Widget3DSubtreeModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(index.isValid());
    auto node = static_cast<Node*>(index.internalPointer());
    Q_ASSERT(node);

    return node->sourceIdx.data(role);
}

QModelIndex Widget3DSubtreeModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    const auto node = mNodeLookup.value(sourceIndex.data(Widget3DModel::IdRole).toString());
    if (!node) {
        return QModelIndex();
    }

    return indexForNode(node);
}

QModelIndex Widget3DSubtreeModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid()) {
        return QModelIndex();
    }

    auto node = static_cast<Node*>(proxyIndex.internalPointer());
    Q_ASSERT(node);

    return node->sourceIdx;
}


QModelIndex Widget3DSubtreeModel::indexForNode(Node *node) const
{
    const int pos = mNodeList.indexOf(node);
    Q_ASSERT(pos > -1);
    return createIndex(pos, 0, node);
}

ObjectId Widget3DSubtreeModel::realObjectId(const QString &objectId) const
{
    Node *node = mNodeLookup[objectId];
    if (!node) {
        return ObjectId();
    }

    return node->sourceIdx.data(ObjectModel::ObjectIdRole).value<ObjectId>();
}
