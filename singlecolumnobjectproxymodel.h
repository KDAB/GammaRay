/*
  singlecolumnobjectproxymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_SINGLECOLUMNOBJECTPROXYMODEL_H
#define GAMMARAY_SINGLECOLUMNOBJECTPROXYMODEL_H

#include "kde/kidentityproxymodel.h"

namespace GammaRay {

class SingleColumnObjectProxyModel : public KIdentityProxyModel
{
  Q_OBJECT
  public:
    explicit SingleColumnObjectProxyModel(QObject *parent = 0);
    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const;
};

}

#endif // GAMMARAY_SINGLECOLUMNOBJECTPROXYMODEL_H
