/*
  widget3dsubtreemodel.h

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

#ifndef WIDGET3DSUBTREEMODEL_H
#define WIDGET3DSUBTREEMODEL_H

#include <QAbstractProxyModel>

#include <common/objectid.h>

#include "widget3dmodel.h"

#include <QPersistentModelIndex>

namespace GammaRay {

class Widget3DSubtreeModel : public QAbstractProxyModel
{
    Q_OBJECT

    Q_PROPERTY(QString rootObjectId READ rootObjectId WRITE setRootObjectId NOTIFY rootObjectIdChanged)

public:
    explicit Widget3DSubtreeModel(QObject *parent = Q_NULLPTR);
    ~Widget3DSubtreeModel();

    void setSourceModel(QAbstractItemModel *sourceModel) Q_DECL_OVERRIDE;

    void setRootObjectId(const QString &rootObject);
    QString rootObjectId() const;

    int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;

    QModelIndex index(int row, int column, const QModelIndex &parent) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;
    bool hasChildren(const QModelIndex &parent) const Q_DECL_OVERRIDE;

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_OVERRIDE;

    ObjectId realObjectId(const QString &objectId) const;

Q_SIGNALS:
    void rootObjectIdChanged();

private Q_SLOTS:
    int sourceRowsInserted(const QModelIndex &parent, int first, int last);
    void sourceRowsAboutToBeRemoved(const QModelIndex &parent, int first, int last);
    void sourceDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void sourceModelReset();
    void sourceLayoutChanged();

private:
    class Node;
    QModelIndex indexForNode(Node *node) const;

    void populate();
    void resetModel();
    bool belongsToModel(const QModelIndex &idx) const;
    bool isParentOf(const QModelIndex &parent, const QModelIndex &child) const;
    QModelIndex findIndexForObject(const QString &objectId) const;

    QString m_rootObject;
    QModelIndex m_rootIndex;

    QList<Node *> mNodeList;
    QHash<QString, Node*> mNodeLookup;

    mutable QSet<QPersistentModelIndex> m_foreignWindows;
};

}

#endif
