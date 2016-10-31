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
    Node()
        : parent(Q_NULLPTR)
    {}

    ~Node()
    {
        qDeleteAll(children);
    }


    QPersistentModelIndex sourceIdx;
    QVector<Node *> children;
    Node *parent;
};

Widget3DSubtreeModel::Widget3DSubtreeModel(QObject *parent)
    : QAbstractItemModel(parent)
    , mSourceModel(Q_NULLPTR)
    , mRoot(Q_NULLPTR)
{
}

Widget3DSubtreeModel::~Widget3DSubtreeModel()
{
    delete mRoot;
}

void Widget3DSubtreeModel::setSourceModel(QAbstractItemModel *newSource)
{
    if (mSourceModel) {
        disconnect(mSourceModel, Q_NULLPTR, this, Q_NULLPTR);
    }

    mSourceModel = newSource;
    Q_EMIT sourceModelChanged();
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

QAbstractItemModel *Widget3DSubtreeModel::sourceModel() const
{
    return mSourceModel;
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
    mNodeLookup.clear();
    delete mRoot;
    mRoot = new Node;
    if (!m_rootObject.isEmpty()) {
        populate();
    }
    endResetModel();
}

void Widget3DSubtreeModel::populate()
{
    QList<QModelIndex> toVisit = { m_rootIndex };
    while (!toVisit.isEmpty()) {
        const auto index = toVisit.takeFirst();
        if (index.data(Widget3DModel::IsWindowRole).toBool()) {
            m_foreignWindows.insert(index);
            if (index != m_rootIndex) {
                continue;
            }
        }

        Node *node = new Node;
        node->sourceIdx = index;
        if (index == m_rootIndex) {
            node->parent = mRoot;
        } else {
            node->parent = mNodeLookup.value(index.parent().data(Widget3DModel::IdRole).toString());
        }
        Q_ASSERT(node->parent);
        node->parent->children.push_back(node);
        mNodeLookup.insert(index.data(Widget3DModel::IdRole).toString(), node);

        for (int i = 0, c = mSourceModel->rowCount(index); i < c; ++i) {
            toVisit.push_front(index.child(i, 0));
        }
    }
}

QModelIndex Widget3DSubtreeModel::findIndexForObject(const QString &objectId) const
{
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

void Widget3DSubtreeModel::sourceRowsInserted(const QModelIndex &parent, int first, int last)
{
    Q_ASSERT(!parent.isValid() || !parent.data(Widget3DModel::IdRole).toString().isEmpty());
    Node *parentNode = Q_NULLPTR;
    if (parent == m_rootIndex) {
        parentNode = mRoot;
    } else {
        parentNode = mNodeLookup.value(parent.data(Widget3DModel::IdRole).toString());
    }
    if (!parentNode) {
        return;
    }

    for (int i = first; i <= last; ++i) {
        const QModelIndex idx = parent.child(i, 0);
        if (idx.data(Widget3DModel::IsWindowRole).toBool()) {
            m_foreignWindows.insert(idx);
            continue;
        }
        if (idx.data(Widget3DModel::IdRole).toString().isEmpty()) {
            continue;
        }

        beginInsertRows(indexForNode(parentNode), parentNode->children.count(), parentNode->children.count());
        Node *node = new Node;
        node->sourceIdx = idx;
        node->parent = parentNode;
        parentNode->children.push_back(node);
        mNodeLookup.insert(idx.data(Widget3DModel::IdRole).toString(), node);
        endInsertRows();
    }
}

void GammaRay::Widget3DSubtreeModel::sourceRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last)
{
    // Check if a parent of our root index is being removed
    QModelIndex i = m_rootIndex.parent();
    while (i.isValid()) {
        if (i == parent) {
            resetModel();
            return;
        }
        i = i.parent();
    }

    Node *parentNode = mNodeLookup.value(parent.data(Widget3DModel::IdRole).toString());
    if (!parentNode) {
        return;
    }

    for (int i = first; i <= last; ++i) {
        const QModelIndex idx = parent.child(i, 0);
        auto lookupIt = mNodeLookup.find(idx.data(Widget3DModel::IdRole).toString());
        if (lookupIt == mNodeLookup.end()) {
            continue;
        }

        const int nodePos = parentNode->children.indexOf((*lookupIt));
        if (nodePos == -1) {
            continue;
        }
        beginRemoveRows(indexForNode(parentNode), nodePos, nodePos);
        mNodeLookup.erase(lookupIt);
        delete parentNode->children.takeAt(nodePos);
        endRemoveRows();
    }
}

void Widget3DSubtreeModel::sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
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
    Node *node = nodeForIndex(parent);
    Q_ASSERT(node);

    return node->children.count();
}

QModelIndex Widget3DSubtreeModel::index(int row, int column, const QModelIndex &parent) const
{
    Node *parentNode = nodeForIndex(parent);
    Q_ASSERT(parentNode);

    if (row >= parentNode->children.count()) {
        return QModelIndex();
    }

    return createIndex(row, column, parentNode);
}

QModelIndex Widget3DSubtreeModel::parent(const QModelIndex &child) const
{
    return indexForNode(static_cast<Node*>(child.internalPointer()));
}

QVariant Widget3DSubtreeModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(index.isValid());
    Node *node = nodeForIndex(index);
    Q_ASSERT(node);

    return node->sourceIdx.data(role);
}

Widget3DSubtreeModel::Node *Widget3DSubtreeModel::nodeForIndex(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return mRoot;
    }

    Q_ASSERT(index.model() == this);
    Node *parentNode = static_cast<Node*>(index.internalPointer());
    return parentNode->children.at(index.row());
}

QModelIndex Widget3DSubtreeModel::indexForNode(Node *node) const
{
    if (node == mRoot) {
        return QModelIndex();
    }

    return createIndex(node->parent->children.indexOf(node), 0, node->parent);
}

ObjectId Widget3DSubtreeModel::realObjectId(const QString &objectId) const
{
    Node *node = mNodeLookup[objectId];
    if (!node) {
        return ObjectId();
    }

    return node->sourceIdx.data(ObjectModel::ObjectIdRole).value<ObjectId>();
}
