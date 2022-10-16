/*
  clientresourcemodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CLIENTRESOURCEMODEL_H
#define GAMMARAY_CLIENTRESOURCEMODEL_H

#include <QFileIconProvider>
#include <QIdentityProxyModel>

#include <QMimeDatabase>

namespace GammaRay {
/**
 * Adds file icons for the resource model.
 * This can't be done server-side since the icon stuff might not exist in a pure QtCore application.
 */
class ClientResourceModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    explicit ClientResourceModel(QObject *parent = nullptr);
    ~ClientResourceModel() override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    QFileIconProvider m_iconProvider;
    QMimeDatabase m_mimeDb;
};
}

#endif // GAMMARAY_CLIENTRESOURCEMODEL_H
