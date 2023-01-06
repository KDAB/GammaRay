/*
  clientdecorationidentityproxymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CLIENTDECORATIONIDENTITYPROXYMODEL_H
#define GAMMARAY_CLIENTDECORATIONIDENTITYPROXYMODEL_H

#include "gammaray_ui_export.h"

#include <QIdentityProxyModel>
#include <QHash>
#include <QIcon>
#include <QPointer>

namespace GammaRay {
class ClassesIconsRepository;

/*! Proxy model for client-side resolution of class icons. */
class GAMMARAY_UI_EXPORT ClientDecorationIdentityProxyModel : public QIdentityProxyModel
{
    Q_OBJECT

public:
    explicit ClientDecorationIdentityProxyModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QPointer<ClassesIconsRepository> m_classesIconsRepository;
    mutable QHash<int, QIcon> m_icons;
};
}

#endif // GAMMARAY_CLIENTDECORATIONIDENTITYPROXYMODEL_H
