/*
  objectmethodmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_OBJECTMETHODMODEL_H
#define GAMMARAY_OBJECTMETHODMODEL_H

#include "metaobjectmodel.h"

#include <common/metatypedeclarations.h>
#include <common/modelroles.h>

#include <QMetaMethod>

namespace GammaRay {

class ObjectMethodModel :
    public MetaObjectModel<QMetaMethod, &QMetaObject::method,
                           &QMetaObject::methodCount, &QMetaObject::methodOffset>
{
  public:
    explicit ObjectMethodModel(QObject *parent = 0);
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QMap< int, QVariant > itemData(const QModelIndex& index) const Q_DECL_OVERRIDE;

  protected:
    QVariant metaData(const QModelIndex &index,
                  const QMetaMethod &method, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QString columnHeader(int index) const Q_DECL_OVERRIDE;
};

}

Q_DECLARE_METATYPE(QMetaMethod)

#endif // GAMMARAY_OBJECTMETHODMODEL_H
