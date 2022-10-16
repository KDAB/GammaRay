/*
  widget3dsubtreemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    explicit Widget3DSubtreeModel(QObject *parent = nullptr);
    ~Widget3DSubtreeModel();

    void setSourceModel(QAbstractItemModel *newSource) override;

    void setRootObjectId(const QString &rootObject);
    QString rootObjectId() const;

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    bool hasChildren(const QModelIndex &parent) const override;

    QVariant data(const QModelIndex &index, int role) const override;

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

    ObjectId realObjectId(const QString &objectId) const;

Q_SIGNALS:
    void rootObjectIdChanged();

private Q_SLOTS:
    int sourceRowsInserted(const QModelIndex &sourceParent, int first, int last);
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
    QHash<QString, Node *> mNodeLookup;

    mutable QSet<QPersistentModelIndex> m_foreignWindows;
};

}

#endif
