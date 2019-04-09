/*
  objectclassinfomodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "objectclassinfomodel.h"

using namespace GammaRay;

ObjectClassInfoModel::ObjectClassInfoModel(QObject *parent)
    : MetaObjectModel<QMetaClassInfo,
                      &QMetaObject::classInfo,
                      &QMetaObject::classInfoCount,
                      &QMetaObject::classInfoOffset>(parent)
{
}

QVariant ObjectClassInfoModel::metaData(const QModelIndex &index, const QMetaClassInfo &classInfo,
                                        int role) const
{
    if (role == Qt::DisplayRole) {
        if (index.column() == 0)
            return classInfo.name();
        if (index.column() == 1)
            return classInfo.value();
    }
    return QVariant();
}

int ObjectClassInfoModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 3;
}

QString GammaRay::ObjectClassInfoModel::columnHeader(int index) const
{
    switch (index) {
    case 0:
        return tr("Name");
    case 1:
        return tr("Value");
    }
    return QString();
}
