/*
  objectmethodmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTMETHODMODEL_H
#define GAMMARAY_OBJECTMETHODMODEL_H

#include "metaobjectmodel.h"

#include <common/metatypedeclarations.h>

#include <QMetaMethod>

namespace GammaRay {
class ObjectMethodModel : public MetaObjectModel<QMetaMethod, &QMetaObject::method,
                                                 &QMetaObject::methodCount,
                                                 &QMetaObject::methodOffset>
{
    Q_OBJECT
public:
    explicit ObjectMethodModel(QObject *parent = nullptr);
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QMap<int, QVariant> itemData(const QModelIndex &index) const override;

protected:
    QVariant metaData(const QModelIndex &index, const QMetaMethod &method,
                      int role = Qt::DisplayRole) const override;
};
}

Q_DECLARE_METATYPE(QMetaMethod)

#endif // GAMMARAY_OBJECTMETHODMODEL_H
