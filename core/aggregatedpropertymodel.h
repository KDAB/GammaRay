/*
  aggregatedpropertymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_AGGREGATEDPROPERTYMODEL_H
#define GAMMARAY_AGGREGATEDPROPERTYMODEL_H

#include <QAbstractItemModel>
#include <QVector>
#include "propertycontrollerextension.h"

namespace GammaRay {

class ObjectPropertyModel;

/** Model that aggregates static and dynamic QObject properties and properties
 *  from our own meta-type system.
 */
class AggregatedPropertyModel : public QAbstractTableModel, public PropertyControllerExtension
{
  Q_OBJECT
public:
  explicit AggregatedPropertyModel(PropertyController *controller);
  ~AggregatedPropertyModel();

  void addModel(ObjectPropertyModel* model);

  void setObject(void *object, const QString &typeName);
  void setObject(QObject *object);
  void setMetaObject(const QMetaObject *metaObject);

  QVariant data(const QModelIndex& index, int role) const;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
  int columnCount(const QModelIndex& parent) const;
  int rowCount(const QModelIndex& parent) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
  Qt::ItemFlags flags(const QModelIndex& index) const;
  QMap<int, QVariant> itemData(const QModelIndex& index) const;

private:
  QModelIndex mapToSource(const QModelIndex &aggregatedIndex) const;
  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;

private slots:
  void sourceModelReset();
  void sourceDataChanged(const QModelIndex &sourceTopLeft, const QModelIndex &sourceBottomRight);

private:
  QVector<ObjectPropertyModel*> m_models;

};

}

#endif // GAMMARAY_AGGREGATEDPROPERTYMODEL_H
