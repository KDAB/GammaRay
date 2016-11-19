/*
  widget3dview.h

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

#include "widget3dwindowmodel.h"

#include "widget3dmodel.h"

#include <QDebug>

namespace GammaRay {
class Widget3DWindowModel::WindowNode
{
public:
    WindowNode(const QModelIndex &idx)
        : sourceIdx(idx)
    {}

    QPersistentModelIndex sourceIdx;
};
}


using namespace GammaRay;

Widget3DWindowModel::Widget3DWindowModel(QObject *parent)
    : QAbstractProxyModel(parent)
{
}

Widget3DWindowModel::~Widget3DWindowModel()
{
}

void Widget3DWindowModel::setSourceModel(QAbstractItemModel *sourceModel_)
{
    if (sourceModel()) {
        disconnect(sourceModel(), 0, this, 0);
    }

    QAbstractProxyModel::setSourceModel(sourceModel_);
    connect(sourceModel_, &QAbstractItemModel::rowsInserted,
            this, &Widget3DWindowModel::sourceModelRowsInserted);
    connect(sourceModel_, &QAbstractItemModel::rowsRemoved,
            this, &Widget3DWindowModel::sourceModelRowsRemoved);
    connect(sourceModel_, &QAbstractItemModel::modelReset,
            this, &Widget3DWindowModel::sourceModelReset);

    beginResetModel();
    qDeleteAll(mNodes);
    mNodes.clear();
    populate();
    endResetModel();
}

QModelIndex Widget3DWindowModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return QModelIndex();
    }

    if (row < 0 || row >= mNodes.count() || column != 0) {
        return QModelIndex();
    }

    return createIndex(row, column, mNodes.at(row));
}

QModelIndex Widget3DWindowModel::parent(const QModelIndex &) const
{
    return QModelIndex(); // list
}


int Widget3DWindowModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : mNodes.count();
}

int Widget3DWindowModel::columnCount(const QModelIndex &) const
{
    return 1;
}

QModelIndex Widget3DWindowModel::indexForNode(WindowNode *node) const
{
    int row = mNodes.indexOf(node);
    Q_ASSERT(row > -1);

    return createIndex(row, 0, node);
}

QModelIndex Widget3DWindowModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    for (WindowNode *node : mNodes) {
        if (node->sourceIdx == sourceIndex) {
            return indexForNode(node);
        }
    }

    return QModelIndex();
}

QModelIndex Widget3DWindowModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid()) {
        return QModelIndex();
    }

    auto node = static_cast<WindowNode*>(proxyIndex.internalPointer());
    if (!node) {
        return QModelIndex();
    }

    return node->sourceIdx;
}

void Widget3DWindowModel::sourceModelRowsInserted(const QModelIndex &parent, int first, int last)
{
    for (int i = first; i <= last; ++i) {
        const QModelIndex srcIdx = sourceModel()->index(i, 0, parent);
        if (srcIdx.data(Widget3DModel::IsWindowRole).toBool()) {
            const int pos = mNodes.size();
            beginInsertRows(QModelIndex(), pos, pos);
            mNodes.push_back(new WindowNode(srcIdx));
            endInsertRows();
        }
    }
}

void Widget3DWindowModel::sourceModelRowsRemoved()
{
    // Iterate over all windows we know and check if any of the indexes has
    // become invalid. We won't ever have that many windows for this to become
    // too slow and we don't have to have a complex code to traverse the entire
    // removed subtree just to check it happened to contain one of our windows.
    for (int i = mNodes.size() - 1; i >= 0; --i) {
        if (!mNodes.at(i)->sourceIdx.isValid()) {
            beginRemoveRows(QModelIndex(), i, i);
            delete mNodes.takeAt(i);
            endRemoveRows();
        }
    }
}

void Widget3DWindowModel::sourceModelReset()
{
    beginResetModel();
    qDeleteAll(mNodes);
    mNodes.clear();
    populate();
    endResetModel();
}

void Widget3DWindowModel::populate()
{
    QVector<QModelIndex> toVisit = { QModelIndex() };
    while (!toVisit.isEmpty()) {
        const auto idx = toVisit.takeFirst();
        if (idx.data(Widget3DModel::IsWindowRole).toBool()) {
            mNodes.push_back(new WindowNode(idx));
        }

        for (int i = 0, cnt = sourceModel()->rowCount(idx); i < cnt; ++i) {
            toVisit.push_front(sourceModel()->index(i, 0, idx));
        }
    }
}
