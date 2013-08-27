/*
  objectclient.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "objectclient.h"
#include "client.h"

using namespace GammaRay;

ObjectClient::ObjectClient(const QString& objectName, QObject* parent) : NetworkObject(objectName, parent)
{
  m_myAddress = Client::instance()->objectAddress(objectName);
  connect(Client::instance(), SIGNAL(objectRegistered(QString,Protocol::ObjectAddress)), SLOT(serverRegistered(QString,Protocol::ObjectAddress)));
  connect(Client::instance(), SIGNAL(objectUnregistered(QString,Protocol::ObjectAddress)),  SLOT(serverUnregistered(QString,Protocol::ObjectAddress)));
  connectToServer();
}

ObjectClient::~ObjectClient()
{
}

void ObjectClient::connectToServer()
{
  if (m_myAddress == Protocol::InvalidObjectAddress)
    return;
  Client::instance()->registerForObject(m_myAddress, this, "newMessage");
}

void ObjectClient::serverRegistered(const QString& objectName, Protocol::ObjectAddress objectAddress)
{
  if (objectName == m_objectName) {
    m_myAddress = objectAddress;
    connectToServer();
  }
}

void ObjectClient::serverUnregistered(const QString& objectName, Protocol::ObjectAddress objectAddress)
{
  Q_UNUSED(objectAddress);
  if (objectName == m_objectName)
    m_myAddress = Protocol::InvalidObjectAddress;
}

#include "objectclient.moc"
