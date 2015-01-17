/*
  serverproxymodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_SERVERPROXYMODEL_H
#define GAMMARAY_SERVERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QVector>

namespace GammaRay {

/** Sort/filter proxy model for server-side use to pass through extra roles in itemData(). */
class ServerProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ServerProxyModel(QObject* parent = 0);

    void addRole(int role);

    QMap<int, QVariant> itemData(const QModelIndex& index) const override;

private:
    QVector<int> m_extraRoles;
};
}

#endif // GAMMARAY_SERVERPROXYMODEL_H
