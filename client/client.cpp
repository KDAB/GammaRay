/*
  client.cpp

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

#include "client.h"

#include <network/message.h>

#include <QDebug>
#include <QTcpSocket>
#include <QHostAddress>

using namespace GammaRay;

Client::Client(QObject* parent)
  : Endpoint(parent)
  , m_versionChecked(false)
{
}

Client::~Client()
{
}

Client* Client::instance()
{
  return static_cast<Client*>(s_instance);
}

void Client::connectToHost(const QString &hostName, quint16 port)
{
  qDebug() << Q_FUNC_INFO << hostName << port;
  QTcpSocket *sock = new QTcpSocket(this);
  connect(sock, SIGNAL(connected()), SLOT(socketConnected()));
  connect(sock, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socketError()));
  sock->connectToHost(hostName, port);
  m_versionChecked = false;
}

void Client::socketConnected()
{
  Q_ASSERT(qobject_cast<QIODevice*>(sender()));
  setDevice(qobject_cast<QIODevice*>(sender()));
}

void Client::socketError()
{
  Q_ASSERT(qobject_cast<QIODevice*>(sender()));
  emit connectionError(qobject_cast<QIODevice*>(sender())->errorString());
}

void Client::messageReceived(const Message& msg)
{
  // server version must be the very first message we get
  if (!m_versionChecked) {
    if (msg.address() != endpointAddress() || msg.type() != Protocol::ServerVersion) {
      qCritical() << "Protocol violation - first message is not the server version.";
      exit(1);
    }
    qint32 serverVersion;
    msg.payload() >> serverVersion;
    if (serverVersion != Protocol::version()) {
      qCritical() << "Server version is" << serverVersion << ", was expecting" << Protocol::version() << " - aborting";
      exit(1);
    }
    m_versionChecked = true;
    emit connectionEstablished();
    return;
  }

  if (msg.address() == endpointAddress()) {
    switch (msg.type()) {
      case Protocol::ObjectAdded:
      {
        QString name;
        Protocol::ObjectAddress addr;
        msg.payload() >> name >> addr;
        qDebug() << Q_FUNC_INFO << "ObjectAdded" << name << addr;
        registerObjectInternal(name, addr);
        break;
      }
      case Protocol::ObjectRemoved:
      {
        QString name;
        msg.payload() >> name;
        unregisterObjectInternal(name);
        break;
      }
      case Protocol::ObjectMapReply:
      {
        QVector<QPair<Protocol::ObjectAddress, QString> > objects;
        msg.payload() >> objects;
        for (QVector<QPair<Protocol::ObjectAddress, QString> >::const_iterator it = objects.constBegin(); it != objects.constEnd(); ++it) {
          if (it->first != endpointAddress())
            registerObjectInternal(it->second, it->first);
        }
        qDebug() << Q_FUNC_INFO << "ObjectMapReply" << objectAddresses();
      }
      default:
        qDebug() << Q_FUNC_INFO << "Got unhandled message:" << msg.type();
        return;
    }
  }

  dispatchMessage(msg);
}

Protocol::ObjectAddress Client::registerObject(const QString &name, QObject *object)
{
  Q_ASSERT(isConnected());
  Protocol::ObjectAddress address = Endpoint::registerObject(name, object);

  Message msg(endpointAddress(), Protocol::ObjectMonitored);
  msg.payload() << address;
  send(msg);

  return address;
}

/// TODO: get rid of this
void Client::registerForObject(Protocol::ObjectAddress objectAddress, QObject* handler, const char* slot)
{
  Q_ASSERT(isConnected());
  registerMessageHandlerInternal(objectAddress, handler, slot);
  Message msg(endpointAddress(), Protocol::ObjectMonitored);
  msg.payload() << objectAddress;
  send(msg);
}

void Client::unregisterForObject(Protocol::ObjectAddress objectAddress)
{
  unregisterMessageHandlerInternal(objectAddress);
  unmonitorObject(objectAddress);
}

void Client::objectDestroyed(Protocol::ObjectAddress objectAddress, const QString &/*objectName*/, QObject */*object*/)
{
  unmonitorObject(objectAddress);
}

void Client::handlerDestroyed(Protocol::ObjectAddress objectAddress, const QString& /*objectName*/)
{
  unmonitorObject(objectAddress);
}

void Client::unmonitorObject(Protocol::ObjectAddress objectAddress)
{
  if (!isConnected())
    return;
  Message msg(endpointAddress(), Protocol::ObjectUnmonitored);
  msg.payload() << objectAddress;
  send(msg);
}

#include "client.moc"
