/*
  singlecolumnobjectproxymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
};
}

#endif // GAMMARAY_SINGLECOLUMNOBJECTPROXYMODEL_H
