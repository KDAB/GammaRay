/*
  aggregatedpropertymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_AGGREGATEDPROPERTYMODEL_H
#define GAMMARAY_AGGREGATEDPROPERTYMODEL_H

#include <QAbstractItemModel>
#include <QVector>

namespace GammaRay {

/** Model that aggregates static and dynamic QObject properties and properties
 *  from our own meta-type system.
 */
class AggregatedPropertyModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  explicit AggregatedPropertyModel(QObject *parent = 0);
  ~AggregatedPropertyModel();

  void addModel(QAbstractItemModel* model);

  QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
  bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
  int columnCount(const QModelIndex& parent) const Q_DECL_OVERRIDE;
  int rowCount(const QModelIndex& parent) const Q_DECL_OVERRIDE;
  QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
  Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
  QMap<int, QVariant> itemData(const QModelIndex& index) const Q_DECL_OVERRIDE;

private:
  QModelIndex mapToSource(const QModelIndex &aggregatedIndex) const;
  QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;

private slots:
  void sourceModelReset();
  void sourceDataChanged(const QModelIndex &sourceTopLeft, const QModelIndex &sourceBottomRight);

private:
  QVector<QAbstractItemModel*> m_models;

};

}

#endif // GAMMARAY_AGGREGATEDPROPERTYMODEL_H
