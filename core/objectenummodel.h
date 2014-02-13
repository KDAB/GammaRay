/*
  objectenummodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_OBJECTENUMMODEL_H
#define GAMMARAY_OBJECTENUMMODEL_H

#include "metaobjectmodel.h"

namespace GammaRay {

class ObjectEnumModel : public MetaObjectModel<QMetaEnum, &QMetaObject::enumerator,
                                               &QMetaObject::enumeratorCount,
                                               &QMetaObject::enumeratorOffset>
{
  public:
    explicit ObjectEnumModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    QString columnHeader(int index) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    QVariant metaData(const QModelIndex &index, const QMetaEnum &enumerator, int role) const;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    QModelIndex parent(const QModelIndex &child) const;
};

}

#endif // GAMMARAY_OBJECTENUMMODEL_H
