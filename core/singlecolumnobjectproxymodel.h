/*
  singlecolumnobjectproxymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
/**
  @file
  This file is part of the GammaRay Plugin API and declares the SingleColumnObjectProxyModel class.

  @brief
  Declares the SingleColumnObjectProxyModel class.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_SINGLECOLUMNOBJECTPROXYMODEL_H
#define GAMMARAY_SINGLECOLUMNOBJECTPROXYMODEL_H

#include <common/objectmodel.h>
#include "util.h"

#if QT_VERSION < QT_VERSION_CHECK(4, 8, 0)
#include <QSortFilterProxyModel>
typedef QSortFilterProxyModel QIdentityProxyModel;
#else
#include <QIdentityProxyModel>
#endif

namespace GammaRay {

/**
 * @brief A QIdentityProxyModel for generic Objects.
 */
class SingleColumnObjectProxyModel : public QIdentityProxyModel
{
  public:
    /**
     * Constructor.
     * @param parent is the parent object for this instance.
     */
    explicit SingleColumnObjectProxyModel(QObject *parent = 0)
      : QIdentityProxyModel(parent)
    {
    }

    /**
     * Returns the data for the specified model.
     * @param proxyIndex is a QModelIndex.
     * @param role is a Qt role.
     *
     * @return on success, a QVariant containing the data;
     *         QVariant() if some anamoly occurs.
     */
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const
    {
      if (proxyIndex.isValid() && role == Qt::DisplayRole && proxyIndex.column() == 0) {
        const QObject *obj = proxyIndex.data(ObjectModel::ObjectRole).value<QObject*>();
        if (obj) {
          return Util::displayString(obj);
        }
      }

      return QIdentityProxyModel::data(proxyIndex, role);
    }
};

}

#endif // GAMMARAY_SINGLECOLUMNOBJECTPROXYMODEL_H
