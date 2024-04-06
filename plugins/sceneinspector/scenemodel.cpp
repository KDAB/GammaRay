/*
  scenemodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "scenemodel.h"

#include <common/objectmodel.h>
#include <common/objectid.h>

#include <QApplication>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QPalette>

using namespace GammaRay;

#define QGV_ITEMTYPE(Type)                                   \
    {                                                        \
        Type t;                                              \
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
            return QStringLiteral("0x%1").arg(QString::number(reinterpret_cast<qlonglong>(item), 16));
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

// QGraphicsItem::childItems() sorts them by stacking order, which takes into account the
// items' insertion order, calls to QGraphicsItem::stackBefore(), and Z-values
// That's too prone to change under our feet without telling us, breaking model invariants
// So always just sort them... by pointer address, for lack of a better idea
namespace {
QList<QGraphicsItem *> sortedChildItems(QGraphicsItem *parent)
{
    auto items = parent->childItems();
    std::sort(items.begin(), items.end());
    return items;
}
}

int SceneModel::rowForItem(QGraphicsItem *item) const
{
    auto parent = item->parentItem();
    if (parent)
        return sortedChildItems(parent).indexOf(item);
    else
        return topLevelItems().indexOf(item);
}

QModelIndex SceneModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
        return {};
    QGraphicsItem *item = static_cast<QGraphicsItem *>(child.internalPointer());
    if (!item->parentItem())
        return QModelIndex();
    const int row = rowForItem(item->parentItem());
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
    return createIndex(row, column, sortedChildItems(parentItem).at(row));
}

QList<QGraphicsItem *> SceneModel::topLevelItems() const
{
    QList<QGraphicsItem *> topLevel;
    if (!m_scene)
        return topLevel;
    const auto allItems = m_scene->items();
    const auto isTopLevel = [](QGraphicsItem *item) { return !item->parentItem(); };
    std::copy_if(allItems.begin(), allItems.end(), std::back_inserter(topLevel), isTopLevel);
    std::sort(topLevel.begin(), topLevel.end());
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
        return QStringLiteral("UserType + %1").arg(itemType - static_cast<int>(QGraphicsItem::UserType));
    }
    return QString::number(itemType);
}
