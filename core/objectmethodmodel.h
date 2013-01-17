/*
  objectmethodmodel.h

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

#ifndef GAMMARAY_OBJECTMETHODMODEL_H
#define GAMMARAY_OBJECTMETHODMODEL_H

#include "metaobjectmodel.h"

#include <QMetaMethod>

namespace GammaRay {

class ObjectMethodModel :
    public MetaObjectModel<QMetaMethod, &QMetaObject::method,
                           &QMetaObject::methodCount, &QMetaObject::methodOffset>
{
  public:
    enum Role {
      MetaMethodRole = Qt::UserRole + 1
    };

    explicit ObjectMethodModel(QObject *parent = 0);
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

  protected:
    QVariant metaData(const QModelIndex &index,
                  const QMetaMethod &method, int role = Qt::DisplayRole) const;
    QString columnHeader(int index) const;
};

}

Q_DECLARE_METATYPE(QMetaMethod)

#endif // GAMMARAY_OBJECTMETHODMODEL_H
