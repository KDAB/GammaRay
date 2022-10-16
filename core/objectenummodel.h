/*
  objectenummodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTENUMMODEL_H
#define GAMMARAY_OBJECTENUMMODEL_H

#include "metaobjectmodel.h"

namespace GammaRay {
class ObjectEnumModel : public MetaObjectModel<QMetaEnum, &QMetaObject::enumerator,
                                               &QMetaObject::enumeratorCount,
                                               &QMetaObject::enumeratorOffset>
{
    Q_OBJECT
public:
    explicit ObjectEnumModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QString columnHeader(int index) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant metaData(const QModelIndex &index, const QMetaEnum &enumerator,
                      int role) const override;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
};
}

#endif // GAMMARAY_OBJECTENUMMODEL_H
