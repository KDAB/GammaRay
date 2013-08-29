/*
  server.cpp

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

#include "server.h"
#include "probe.h"

#include <network/protocol.h>
#include <network/message.h>

#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>
#include <QMetaMethod>
#include <QNetworkInterface>
#include <QSignalSpy>

using namespace GammaRay;

Server::Server(QObject *parent) : 
  Endpoint(parent),
  m_tcpServer(new QTcpServer(this)),
  m_nextAddress(endpointAddress()),
  m_broadcastTimer(new QTimer(this)),
  m_broadcastSocket(new QUdpSocket(this))
{
  connect(m_tcpServer, SIGNAL(newConnection()), SLOT(newConnection()));
  m_tcpServer->listen(QHostAddress::Any, defaultPort());

  m_broadcastTimer->setInterval(5 * 1000);
  m_broadcastTimer->setSingleShot(false);
  m_broadcastTimer->start();
  connect(m_broadcastTimer, SIGNAL(timeout()), SLOT(broadcast()));
  connect(this, SIGNAL(disconnected()), m_broadcastTimer, SLOT(start()));
}

Server::~Server()
{
}

Server* Server::instance()
{
  Q_ASSERT(s_instance);
  return static_cast<Server*>(s_instance);
}


void Server::newConnection()
{
  if (isConnected()) {
    qDebug() << Q_FUNC_INFO << "connected already, refusing incoming connection.";
    m_tcpServer->nextPendingConnection()->close();
    return;
  }

  qDebug() << Q_FUNC_INFO;
  m_broadcastTimer->stop();
  setDevice(m_tcpServer->nextPendingConnection());

  // send greeting message for protocol version check
  {
    Message msg(endpointAddress(), Protocol::ServerVersion);
    msg.payload() << Protocol::version();
    send(msg);
  }

  {
    Message msg(endpointAddress(), Protocol::ObjectMapReply);
    msg.payload() << objectAddresses();
    send(msg);
  }
}

void Server::messageReceived(const Message& msg)
{
  if (msg.address() == endpointAddress()) {
    switch (msg.type()) {
      case Protocol::ObjectMonitored:
      case Protocol::ObjectUnmonitored:
      {
        Protocol::ObjectAddress addr;
        msg.payload() >> addr;
        Q_ASSERT(addr > Protocol::InvalidObjectAddress);
        const QHash<Protocol::ObjectAddress, QPair<QObject*, QByteArray> >::const_iterator it = m_monitorNotifiers.constFind(addr);
        if (it == m_monitorNotifiers.constEnd())
          break;
        qDebug() << Q_FUNC_INFO << "un/monitor" << addr;
        QMetaObject::invokeMethod(it.value().first, it.value().second, Q_ARG(bool, msg.type() == Protocol::ObjectMonitored));
        break;
      }
    }
  } else {
    dispatchMessage(msg);
  }
}

void Server::invokeObject(const QString &objectName, const char *method, const QVariantList &args) const
{
  Endpoint::invokeObject(objectName, method, args);

  QObject* object = ObjectBroker::objectInternal(objectName);
  Q_ASSERT(object);
  // also invoke locally for in-process mode
  invokeObjectLocal(object, method, args);
}

Protocol::ObjectAddress Server::registerObject(const QString &name, QObject *object)
{
  registerObjectInternal(name, ++m_nextAddress);
  Protocol::ObjectAddress address = Endpoint::registerObject(name, object);
  Q_ASSERT(m_nextAddress);

  if (isConnected()) {
    Message msg(endpointAddress(), Protocol::ObjectAdded);
    msg.payload() <<  name << m_nextAddress;
    send(msg);
  }

  const QMetaObject *meta = object->metaObject();

  QHash<int, QSignalSpy*>& signalForwards = m_signalForwards[object];
  for(int i = 0; i < meta->methodCount(); ++i) {
    QMetaMethod method = meta->method(i);
    if (method.methodType() == QMetaMethod::Signal) {
      QByteArray signature = method.signature();
      // simulate SIGNAL() macro by prepending magic number.
      signature.prepend(QSIGNAL_CODE);
      QSignalSpy *spy = new QSignalSpy(object, signature);
      spy->setParent(object);
      signalForwards[i] = spy;
      connect(object, signature, this, SLOT(forwardSignal()));
    }
  }

  return address;
}

void Server::forwardSignal() const
{
  Q_ASSERT(sender());
  QSignalSpy *spy = m_signalForwards.value(sender()).value(senderSignalIndex());
  Q_ASSERT(spy->count() == 1);

  if (!isConnected()) {
    spy->clear();
    return;
  }

  QByteArray name = spy->signal();
  // get the name of the function to invoke, excluding the parens and function arguments.
  name = name.mid(0, name.indexOf('('));
  QVariantList args = spy->takeFirst();
  Endpoint::invokeObject(sender()->objectName(), name, args);
}

Protocol::ObjectAddress Server::registerObject(const QString& objectName, QObject* receiver, const char* messageHandlerName, const char* monitorNotifier)
{
  registerObjectInternal(objectName, ++m_nextAddress);
  Q_ASSERT(m_nextAddress);
  registerMessageHandlerInternal(m_nextAddress, receiver, messageHandlerName);

  if (monitorNotifier)
    m_monitorNotifiers.insert(m_nextAddress, qMakePair<QObject*, QByteArray>(receiver, monitorNotifier));

  if (isConnected()) {
    Message msg(endpointAddress(), Protocol::ObjectAdded);
    msg.payload() <<  objectName << m_nextAddress;
    send(msg);
  }

  return m_nextAddress;
}

void Server::handlerDestroyed(Protocol::ObjectAddress objectAddress, const QString& objectName)
{
  unregisterObjectInternal(objectName);
  m_monitorNotifiers.remove(objectAddress);

  if (isConnected()) {
    Message msg(endpointAddress(), Protocol::ObjectRemoved);
    msg.payload() << objectName;
    send(msg);
  }
}

void Server::objectDestroyed(Protocol::ObjectAddress objectAddress, const QString &objectName, QObject *object)
{
  m_signalForwards.remove(object);

  unregisterObjectInternal(objectName);

  if (isConnected()) {
    Message msg(endpointAddress(), Protocol::ObjectRemoved);
    msg.payload() << objectName;
    send(msg);
  }
}

void Server::broadcast()
{
  QString myAddress;
  foreach (const QHostAddress &addr, QNetworkInterface::allAddresses()) {
    if (addr == QHostAddress::LocalHost || addr == QHostAddress::LocalHostIPv6 || !addr.scopeId().isEmpty())
      continue;
    myAddress = addr.toString();
    break;
  }

  QByteArray datagram;
  QDataStream stream(&datagram, QIODevice::WriteOnly);
  stream << Protocol::broadcastFormatVersion();
  stream << Protocol::version();
  stream << myAddress;
  stream << defaultPort(); // might change for multiple instances on the same machine
  stream << m_label; // TODO integrate hostname
  m_broadcastSocket->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, broadcastPort());
}

void Server::setLabel(const QString& label)
{
  m_label = label;
}

#include "server.moc"
