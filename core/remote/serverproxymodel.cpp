/*
  serverproxymodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "serverproxymodel.h"

#include <QDebug>

using namespace GammaRay;

ServerProxyModel::ServerProxyModel(QObject *parent): QSortFilterProxyModel(parent)
{
}

void ServerProxyModel::addRole(int role)
{
    m_extraRoles.push_back(role);
}

QMap< int, QVariant > ServerProxyModel::itemData(const QModelIndex &index) const
{
    const QModelIndex sourceIndex = mapToSource(index);
    auto d = sourceModel()->itemData(sourceIndex);
    foreach (int role, m_extraRoles) {
        d.insert(role, sourceIndex.data(role));
    }
    return d;
}
