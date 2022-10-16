/*
  objectclassinfomodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTCLASSINFOMODEL_H
#define GAMMARAY_OBJECTCLASSINFOMODEL_H

#include "metaobjectmodel.h"
#include <QMetaClassInfo>

namespace GammaRay {
class ObjectClassInfoModel : public MetaObjectModel<QMetaClassInfo,
                                                    &QMetaObject::classInfo,
                                                    &QMetaObject::classInfoCount,
                                                    &QMetaObject::classInfoOffset>
{
    Q_OBJECT
public:
    explicit ObjectClassInfoModel(QObject *parent = nullptr);
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant metaData(const QModelIndex &index, const QMetaClassInfo &classInfo,
                      int role) const override;
    QString columnHeader(int index) const override;
};
}

#endif // GAMMARAY_OBJECTCLASSINFOMODEL_H
