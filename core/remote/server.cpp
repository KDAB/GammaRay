/*
  server.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "probesettings.h"
#include "multisignalmapper.h"

#include <common/protocol.h>
#include <common/message.h>

#include <QTcpServer>
#include <QTcpSocket>
#include <QUdpSocket>
#include <QTimer>
#include <QMetaMethod>
#include <QNetworkInterface>

#include <iostream>

using namespace GammaRay;
using namespace std;

Server::Server(QObject *parent) :
  Endpoint(parent),
  m_tcpServer(new QTcpServer(this)),
  m_nextAddress(endpointAddress()),
  m_broadcastTimer(new QTimer(this)),
  m_broadcastSocket(new QUdpSocket(this)),
  m_signalMapper(new MultiSignalMapper(this))
{
  if (!ProbeSettings::value("RemoteAccessEnabled", true).toBool())
    return;

  const QHostAddress address(ProbeSettings::value("TCPServer", QLatin1String("0.0.0.0")).toString());

  connect(m_tcpServer, SIGNAL(newConnection()), SLOT(newConnection()));

  // try the default port first, and fall back to a random port otherwise
  if (!m_tcpServer->listen(address, defaultPort()))
    m_tcpServer->listen(address, 0);

  // broadcast announcement only if we are actually listinging to remote connections
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  if (address.toString() != "127.0.0.1" && address.toString() != "::1") {
#else
  if (!address.isLoopback()) {
#endif
    m_broadcastTimer->setInterval(5 * 1000);
    m_broadcastTimer->setSingleShot(false);
    m_broadcastTimer->start();
    connect(m_broadcastTimer, SIGNAL(timeout()), SLOT(broadcast()));
    connect(this, SIGNAL(disconnected()), m_broadcastTimer, SLOT(start()));
  }

  connect(m_signalMapper, SIGNAL(signalEmitted(QObject*,int,QVector<QVariant>)),
          this, SLOT(forwardSignal(QObject*,int,QVector<QVariant>)));
}

Server::~Server()
{
}

Server* Server::instance()
{
  Q_ASSERT(s_instance);
  return static_cast<Server*>(s_instance);
}

bool Server::isRemoteClient() const
{
  return false;
}

QString Server::serverAddress() const
{
  return QHostAddress(QHostAddress::LocalHost).toString();
}

void Server::newConnection()
{
  if (isConnected()) {
    cerr << Q_FUNC_INFO << " connected already, refusing incoming connection." << endl;
    m_tcpServer->nextPendingConnection()->close();
    return;
  }

  m_broadcastTimer->stop();
  setDevice(m_tcpServer->nextPendingConnection());

  // send greeting message for protocol version check
  {
    Message msg(endpointAddress(), Protocol::ServerVersion);
    msg.payload() << Protocol::version();
    send(msg);
  }

  {
    Message msg(endpointAddress(), Protocol::ServerInfo);
    msg.payload() << label(); // TODO: expand with anything else needed here: Qt/GammaRay version, hostname, that kind of stuff
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
        //cout << Q_FUNC_INFO << " un/monitor " << (int)addr << endl;
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
  for(int i = 0; i < meta->methodCount(); ++i) {
    const QMetaMethod method = meta->method(i);
    if (method.methodType() == QMetaMethod::Signal) {
      m_signalMapper->connectToSignal(object, method);
    }
  }

  return address;
}

void Server::forwardSignal(QObject* sender, int signalIndex, const QVector< QVariant >& args)
{
  if (!isConnected())
    return;

  Q_ASSERT(sender);
  Q_ASSERT(signalIndex >= 0);
  const QMetaMethod signal = sender->metaObject()->method(signalIndex);
  Q_ASSERT(signal.methodType() == QMetaMethod::Signal);

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  QByteArray name = signal.signature();
#else
  QByteArray name = signal.methodSignature();
#endif
  // get the name of the function to invoke, excluding the parens and function arguments.
  name = name.mid(0, name.indexOf('('));

  QVariantList v;
  foreach(const QVariant &arg, args)
    v.push_back(arg);
  Endpoint::invokeObject(sender->objectName(), name, v);
}

Protocol::ObjectAddress Server::registerObject(const QString& objectName, QObject* receiver, const char* messageHandlerName)
{
  registerObjectInternal(objectName, ++m_nextAddress);
  Q_ASSERT(m_nextAddress);
  registerMessageHandlerInternal(m_nextAddress, receiver, messageHandlerName);

  if (isConnected()) {
    Message msg(endpointAddress(), Protocol::ObjectAdded);
    msg.payload() <<  objectName << m_nextAddress;
    send(msg);
  }

  return m_nextAddress;
}

void Server::registerMonitorNotifier(Protocol::ObjectAddress address, QObject* receiver, const char* monitorNotifier)
{
  Q_ASSERT(address != Protocol::InvalidObjectAddress);
  Q_ASSERT(receiver);
  Q_ASSERT(monitorNotifier);

  m_monitorNotifiers.insert(address, qMakePair<QObject*, QByteArray>(receiver, monitorNotifier));
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

void Server::objectDestroyed(Protocol::ObjectAddress /*objectAddress*/, const QString &objectName, QObject *object)
{
  Q_UNUSED(object);
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
  stream << port();
  stream << label(); // TODO integrate hostname
  m_broadcastSocket->writeDatagram(datagram.data(), datagram.size(), QHostAddress::Broadcast, broadcastPort());
}

quint16 Server::port() const
{
  return m_tcpServer->serverPort();
}
