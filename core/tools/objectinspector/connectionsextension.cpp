/*
  connectionsextension.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "connectionsextension.h"
#include "inboundconnectionsmodel.h"
#include "outboundconnectionsmodel.h"

#include <core/connectionfilterproxymodel.h>
#include <core/probe.h>
#include <core/propertycontroller.h>

#include <common/tools/objectinspector/connectionsmodelroles.h>

using namespace GammaRay;

ConnectionsExtension::ConnectionsExtension(PropertyController* controller):
  ConnectionsExtensionInterface(controller->objectBaseName() + ".connectionsExtension", controller),
  PropertyControllerExtension(controller->objectBaseName() + ".connections")
{
#ifndef USE_QT_CONNECTIONS_LIST
  m_inboundModel = new ConnectionFilterProxyModel(controller);
  m_outboundModel = new ConnectionFilterProxyModel(controller);

  m_inboundModel->setFilterOnReceiver(true);
  m_outboundModel->setFilterOnSender(true);

  m_inboundModel->setSourceModel(Probe::instance()->connectionModel());
  m_outboundModel->setSourceModel(Probe::instance()->connectionModel());
#else
  m_inboundModel = new InboundConnectionsModel(controller);
  m_outboundModel = new OutboundConnectionsModel(controller);
#endif

  controller->registerModel(m_inboundModel, "inboundConnections");
  controller->registerModel(m_outboundModel, "outboundConnections");
}

ConnectionsExtension::~ConnectionsExtension()
{
}

bool ConnectionsExtension::setQObject(QObject* object)
{
#ifndef USE_QT_CONNECTIONS_LIST
  m_inboundModel->filterReceiver(object);
  m_outboundModel->filterSender(object);
#else
  m_inboundModel->setObject(object);
  m_outboundModel->setObject(object);
#endif

  return true;
}

void ConnectionsExtension::navigateToSender(int modelRow)
{
  const QModelIndex index = m_inboundModel->index(modelRow, 0);
  QObject* sender = index.data(ConnectionsModelRoles::EndpointRole).value<QObject*>();
  if (!sender)
    return;
  Probe::instance()->selectObject(sender);
}

void ConnectionsExtension::navigateToReceiver(int modelRow)
{
  const QModelIndex index = m_outboundModel->index(modelRow, 0);
  QObject* receiver = index.data(ConnectionsModelRoles::EndpointRole).value<QObject*>();
  if (!receiver)
    return;
  Probe::instance()->selectObject(receiver);
}
