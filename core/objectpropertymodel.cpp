/*
  objectpropertymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "objectpropertymodel.h"

#include <common/propertymodel.h>

#include <QMetaProperty>
#include <QTimer>

using namespace GammaRay;

ObjectPropertyModel::ObjectPropertyModel(QObject *parent)
  : QAbstractTableModel(parent),
    m_metaObject(0),
    m_updateTimer(new QTimer(this))
{
  connect(m_updateTimer, SIGNAL(timeout()), SLOT(doEmitChanged()));
  m_updateTimer->setSingleShot(true);
}

void ObjectPropertyModel::setObject(QObject *object)
{
  if (m_obj == object)
    return;

  beginResetModel();
  if (m_obj) {
    unmonitorObject(m_obj.data());
    disconnect(m_obj.data(), SIGNAL(destroyed(QObject*)), this, SLOT(slotReset()));
  }
  m_obj = object;
  m_metaObject = object ? object->metaObject() : 0;
  if (object) {
    connect(object, SIGNAL(destroyed(QObject*)), SLOT(slotReset()));
    monitorObject(object);
  }
  endResetModel();
}

QMap< int, QVariant > ObjectPropertyModel::itemData(const QModelIndex& index) const
{
  QMap<int, QVariant> d = QAbstractItemModel::itemData(index);
  d.insert(PropertyModel::ActionRole, data(index, PropertyModel::ActionRole));
  d.insert(PropertyModel::AppropriateToolRole, data(index, PropertyModel::AppropriateToolRole));
  return d;
}

void ObjectPropertyModel::updateAll()
{
  if (m_updateTimer->isActive()) {
    return;
  }
  m_updateTimer->start(100);
}

void ObjectPropertyModel::doEmitChanged()
{
  emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

void ObjectPropertyModel::slotReset()
{
  reset();
}
