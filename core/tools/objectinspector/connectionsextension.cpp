/*
  connectionsextension.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>

#include "connectionsextension.h"
#include "inboundconnectionsmodel.h"
#include "outboundconnectionsmodel.h"

#include <core/probe.h>
#include <core/propertycontroller.h>

#include <common/tools/objectinspector/connectionsmodelroles.h>

using namespace GammaRay;

ConnectionsExtension::ConnectionsExtension(PropertyController *controller)
    : ConnectionsExtensionInterface(
        controller->objectBaseName() + ".connectionsExtension", controller)
    , PropertyControllerExtension(controller->objectBaseName() + ".connections")
{
    m_inboundModel = new InboundConnectionsModel(controller);
    m_outboundModel = new OutboundConnectionsModel(controller);

    controller->registerModel(m_inboundModel, QStringLiteral("inboundConnections"));
    controller->registerModel(m_outboundModel, QStringLiteral("outboundConnections"));
}

ConnectionsExtension::~ConnectionsExtension() = default;

bool ConnectionsExtension::setQObject(QObject *object)
{
    m_inboundModel->setObject(object);
    m_outboundModel->setObject(object);

    return true;
}

void ConnectionsExtension::navigateToSender(int modelRow)
{
    const QModelIndex index = m_inboundModel->index(modelRow, 0);
    QObject *sender = index.data(ConnectionsModelRoles::EndpointRole).value<QObject *>();
    if (!sender)
        return;
    Probe::instance()->selectObject(sender);
}

void ConnectionsExtension::navigateToReceiver(int modelRow)
{
    const QModelIndex index = m_outboundModel->index(modelRow, 0);
    QObject *receiver = index.data(ConnectionsModelRoles::EndpointRole).value<QObject *>();
    if (!receiver)
        return;
    Probe::instance()->selectObject(receiver);
}
