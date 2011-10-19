/*
  scenemodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include <qgraphicsitem.h>
#include <qgraphicsscene.h>
#include <qdebug.h>

using namespace GammaRay;

SceneModel::SceneModel(QObject *parent)
  : QAbstractItemModel(parent),
    m_scene(0)
{
}

void SceneModel::setScene(QGraphicsScene *scene)
{
  m_scene = scene;
//   qDebug() << "total amount of graphics items:" << m_scene->items().size();
  reset();
}

QVariant SceneModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid()) {
    return QVariant();
  }
  QGraphicsItem *item = static_cast<QGraphicsItem*>(index.internalPointer());

  if (item && role == Qt::DisplayRole) {
    QGraphicsObject *obj = item->toGraphicsObject();
    if (index.column() == 0) {
      if (obj && !obj->objectName().isEmpty()) {
        return obj->objectName();
      }
      return QLatin1String("0x") + QString::number(reinterpret_cast<qlonglong>(item), 16);
    } else if (index.column() == 1) {
      if (obj) {
        return obj->metaObject()->className();
      }
      return QString::number(item->type());
    }
  } else if (role == SceneItemRole) {
    return QVariant::fromValue(item);
  } else if (item && role == Qt::ForegroundRole) {
    if (!item->isVisible()) {
      return Qt::gray;
    }
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
  if (!m_scene) {
    return 0;
  }
  if (parent.isValid()) {
    QGraphicsItem* item = static_cast<QGraphicsItem*>(parent.internalPointer());
    if (item) {
      return item->childItems().size();
    } else {
      return 0;
    }
  }
  return topLevelItems().size();
}

QModelIndex SceneModel::parent(const QModelIndex &child) const
{
  if (!child.isValid()) {
    return QModelIndex();
  }
  QGraphicsItem *item = static_cast<QGraphicsItem*>(child.internalPointer());
  if (!item->parentItem()) {
    return QModelIndex();
  }
  int row = item->parentItem()->childItems().indexOf(item);
  return createIndex(row, 0, item->parentItem());
}

QModelIndex SceneModel::index(int row, int column, const QModelIndex &parent) const
{
  if (!parent.isValid() && row >= 0 && row < topLevelItems().size()) {
    return createIndex(row, column, topLevelItems().at(row));
  }
  QGraphicsItem *parentItem = static_cast<QGraphicsItem*>(parent.internalPointer());
  if (!parentItem || row < 0 || row >= parentItem->childItems().size()) {
    return QModelIndex();
  }
  return createIndex(row, column, parentItem->childItems().at(row));
}

QList<QGraphicsItem*> SceneModel::topLevelItems() const
{
  QList<QGraphicsItem*> topLevel;
  if (!m_scene) {
    return topLevel;
  }
  Q_FOREACH (QGraphicsItem *item, m_scene->items()) {
    if (!item->parentItem()) {
      topLevel.push_back(item);
    }
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

#include "scenemodel.moc"
