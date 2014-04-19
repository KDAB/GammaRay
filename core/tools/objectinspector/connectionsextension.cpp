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

#include <core/connectionfilterproxymodel.h>
#include <core/probe.h>
#include <core/propertycontroller.h>

using namespace GammaRay;

ConnectionsExtension::ConnectionsExtension(PropertyController* controller):
  PropertyControllerExtension(controller->objectBaseName() + ".connections"),
  m_inboundModel(new ConnectionFilterProxyModel(controller)),
  m_outboundModel(new ConnectionFilterProxyModel(controller))
{
  m_inboundModel->setFilterOnReceiver(true);
  m_outboundModel->setFilterOnSender(true);

  m_inboundModel->setSourceModel(Probe::instance()->connectionModel());
  m_outboundModel->setSourceModel(Probe::instance()->connectionModel());

  controller->registerModel(m_inboundModel, "inboundConnections2"); // TODO fix name once the old ones are gone
  controller->registerModel(m_outboundModel, "outboundConnections2");
}

ConnectionsExtension::~ConnectionsExtension()
{
}

bool ConnectionsExtension::setQObject(QObject* object)
{
  m_inboundModel->filterReceiver(object);
  m_outboundModel->filterSender(object);

  return true;
}
