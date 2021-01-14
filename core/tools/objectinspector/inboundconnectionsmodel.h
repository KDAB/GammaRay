/*
  inboundconnectionsmodel.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_OBJECTINSPECTOR_INBOUNDCONNECTIONSMODEL_H
#define GAMMARAY_OBJECTINSPECTOR_INBOUNDCONNECTIONSMODEL_H

#include "abstractconnectionsmodel.h"

namespace GammaRay {
/** List of inbound connections on a given QObject. */
class InboundConnectionsModel : public AbstractConnectionsModel
{
    Q_OBJECT
public:
    explicit InboundConnectionsModel(QObject *parent = nullptr);
    ~InboundConnectionsModel() override;

    void setObject(QObject *object) override;

    static QVector<Connection> inboundConnectionsForObject(QObject *object);

    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const override;
};
}

#endif // GAMMARAY_INBOUNDCONNECTIONSMODEL_H
