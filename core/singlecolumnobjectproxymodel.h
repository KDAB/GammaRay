/*
  singlecolumnobjectproxymodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/
/**
  @file
  This file is part of the GammaRay Plugin API and declares the SingleColumnObjectProxyModel class.

  @brief
  Declares the SingleColumnObjectProxyModel class.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_SINGLECOLUMNOBJECTPROXYMODEL_H
#define GAMMARAY_SINGLECOLUMNOBJECTPROXYMODEL_H

#include "gammaray_core_export.h"

#include <QIdentityProxyModel>

namespace GammaRay {
/**
 * @brief A QIdentityProxyModel for generic Objects.
 */
class GAMMARAY_CORE_EXPORT SingleColumnObjectProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    /**
     * Constructor.
     * @param parent is the parent object for this instance.
     */
    explicit SingleColumnObjectProxyModel(QObject *parent = nullptr);

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * Returns the data for the specified model.
     * @param proxyIndex is a QModelIndex.
     * @param role is a Qt role.
     *
     * @return on success, a QVariant containing the data;
     *         QVariant() if some anamoly occurs.
     */
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override;

    QMap<int, QVariant> itemData(const QModelIndex &proxyIndex) const override;
};
}

#endif // GAMMARAY_SINGLECOLUMNOBJECTPROXYMODEL_H
