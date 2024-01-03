/*
  objectclassinfomodel.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
