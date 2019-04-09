/*
  scenemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "scenemodel.h"

#include <common/objectmodel.h>
#include <common/objectid.h>

#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPalette>

using namespace GammaRay;

#define QGV_ITEMTYPE(Type) \
    { \
        Type t; \
        m_typeNames.insert(t.type(), QStringLiteral(#Type)); \
    }

SceneModel::SceneModel(QObject *parent)
    : QAbstractItemModel(parent)
    , m_scene(nullptr)
{
    QGV_ITEMTYPE(QGraphicsLineItem)
    QGV_ITEMTYPE(QGraphicsPixmapItem)
    QGV_ITEMTYPE(QGraphicsRectItem)
    QGV_ITEMTYPE(QGraphicsEllipseItem)
    QGV_ITEMTYPE(QGraphicsPathItem)
    QGV_ITEMTYPE(QGraphicsPolygonItem)
    QGV_ITEMTYPE(QGraphicsSimpleTextItem)
    QGV_ITEMTYPE(QGraphicsItemGroup)
}

void SceneModel::setScene(QGraphicsScene *scene)
{
    beginResetModel();
    m_scene = scene;
    endResetModel();
}

QGraphicsScene *SceneModel::scene() const
{
    return m_scene;
}

QVariant SceneModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();
    QGraphicsItem *item = static_cast<QGraphicsItem *>(index.internalPointer());

    if (item && role == Qt::DisplayRole) {
        QGraphicsObject *obj = item->toGraphicsObject();
        if (index.column() == 0) {
            if (obj && !obj->objectName().isEmpty())
                return obj->objectName();
            return
                QStringLiteral("0x%1").
                arg(QString::number(reinterpret_cast<qlonglong>(item), 16));
        } else if (index.column() == 1) {
            if (obj)
                return obj->metaObject()->className();
            return typeName(item->type());
        }
    } else if (role == SceneItemRole) {
        return QVariant::fromValue(item);
    } else if (item && role == Qt::ForegroundRole) {
        if (!item->isVisible())
            return qApp->palette().color(QPalette::Disabled, QPalette::Text);
    } else if (item && role == ObjectModel::ObjectIdRole) {
        // TODO also handle the non-QObject case
        return QVariant::fromValue(ObjectId(item->toGraphicsObject()));
    }
    return QVariant();
}

int SceneModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 2;
}

int SceneModel::rowCount(const QModelIndex &parent) const
{
    if (!m_scene)
        return 0;
    if (parent.isValid()) {
        if (parent.column() != 0)
            return 0;
        QGraphicsItem *item = static_cast<QGraphicsItem *>(parent.internalPointer());
        if (item)
            return item->childItems().size();
        else
            return 0;
    }
    return topLevelItems().size();
}

QModelIndex SceneModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return {};
    QGraphicsItem *item = static_cast<QGraphicsItem *>(child.internalPointer());
    if (!item->parentItem())
        return QModelIndex();
    int row = 0;
    if (item->parentItem()->parentItem())
        row = item->parentItem()->parentItem()->childItems().indexOf(item->parentItem());
    return createIndex(row, 0, item->parentItem());
}

QModelIndex SceneModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column < 0 || column >= columnCount())
        return {};
    if (!parent.isValid() && row >= 0 && row < topLevelItems().size())
        return createIndex(row, column, topLevelItems().at(row));
    QGraphicsItem *parentItem = static_cast<QGraphicsItem *>(parent.internalPointer());
    if (!parentItem || row < 0 || row >= parentItem->childItems().size())
        return QModelIndex();
    return createIndex(row, column, parentItem->childItems().at(row));
}

QList<QGraphicsItem *> SceneModel::topLevelItems() const
{
    QList<QGraphicsItem *> topLevel;
    if (!m_scene)
        return topLevel;
    Q_FOREACH(QGraphicsItem *item, m_scene->items()) {
        if (!item->parentItem())
            topLevel.push_back(item);
    }
    return topLevel;
}

QVariant SceneModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section) {
        case 0:
            return tr("Item");
        case 1:
            return tr("Type");
        }
    }
    return QAbstractItemModel::headerData(section, orientation, role);
}

QString SceneModel::typeName(int itemType) const
{
    auto it = m_typeNames.find(itemType);
    if (it != m_typeNames.end())
        return it.value();
    if (itemType == QGraphicsItem::UserType)
        return QStringLiteral("UserType");
    if (itemType > QGraphicsItem::UserType) {
        return
            QStringLiteral("UserType + %1").
            arg(itemType - static_cast<int>(QGraphicsItem::UserType));
    }
    return QString::number(itemType);
}
