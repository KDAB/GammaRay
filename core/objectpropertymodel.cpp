/*
  objectpropertymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "objectpropertymodel.h"

#include <QMetaProperty>
#include <QTimer>

using namespace GammaRay;

ObjectPropertyModel::ObjectPropertyModel(QObject *parent)
  : QAbstractTableModel(parent),
    m_updateTimer(new QTimer(this))
{
  connect(m_updateTimer, SIGNAL(timeout()), SLOT(doEmitChanged()));
  m_updateTimer->setSingleShot(true);
}

void ObjectPropertyModel::setObject(QObject *object)
{
  if (m_obj) {
    disconnect(m_obj.data(), 0, this, SLOT(updateAll()));
    disconnect(m_obj.data(), 0, this, SLOT(slotReset()));
  }
  m_obj = object;
  if (object) {
    connect(object, SIGNAL(destroyed(QObject*)), SLOT(slotReset()));
    for (int i = 0; i < object->metaObject()->propertyCount(); ++i) {
      const QMetaProperty prop = object->metaObject()->property(i);
      if (prop.hasNotifySignal()) {
        connect(object, QByteArray("2") +
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            prop.notifySignal().signature()
#else
            prop.notifySignal().methodSignature()
#endif
            , SLOT(updateAll()));
      }
    }
  }
  reset();
}

QVariant ObjectPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    switch (section) {
    case 0:
      return tr("Property");
    case 1:
      return tr("Value");
    case 2:
      return tr("Type");
    case 3:
      return tr("Class");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
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

