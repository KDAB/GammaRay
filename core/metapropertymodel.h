/*
  metapropertymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_METAPROPERTYMODEL_H
#define GAMMARAY_METAPROPERTYMODEL_H

#include "gammaray_core_export.h"

#include <QAbstractTableModel>

namespace GammaRay {

class MetaObject;

/** Model showing non-QObject object properties.
 * @todo needs better name, but ObjectPropertyModel is already in use...
 * @todo maybe it's a good to merge those to anyway?
 */
class GAMMARAY_CORE_EXPORT MetaPropertyModel : public QAbstractTableModel
{
  Q_OBJECT
  public:
    explicit MetaPropertyModel(QObject *parent = 0);

    /** Sets the object that should be represented by this model. */
    void setObject(void *object, const QString &typeName);
    void setObject(QObject *object);

    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    QMap< int, QVariant > itemData(const QModelIndex& index) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex &index) const;

  private:
    MetaObject *m_metaObject;
    void *m_object;
};

}

#endif // GAMMARAY_METAPROPERTYMODEL_H
