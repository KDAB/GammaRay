/*
  endpoint.cpp

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

#include "endpoint.h"
#include "message.h"

#include <QDebug>

using namespace GammaRay;

Endpoint* Endpoint::s_instance = 0;

Endpoint::Endpoint(QObject* parent): QObject(parent), m_socket(0), m_myAddress(Protocol::InvalidObjectAddress +1)
{
  Q_ASSERT(!s_instance);
  s_instance = this;

  ObjectInfo *endpointObj = new ObjectInfo;
  endpointObj->address = m_myAddress;
  endpointObj->name = QLatin1String("com.kdab.GammaRay.Server");
  // TODO: we could set this as message handler here and use the same dispatch mechanism
  insertObjectInfo(endpointObj);
}

Endpoint::~Endpoint()
{
  for (QHash<Protocol::ObjectAddress, ObjectInfo*>::const_iterator it = m_addressMap.constBegin(); it != m_addressMap.constEnd(); ++it)
    delete it.value();
}

Endpoint* Endpoint::instance()
{
  return s_instance;
}

void Endpoint::send(const Message& msg)
{
  Q_ASSERT(s_instance);
  msg.write(s_instance->m_socket);
}

bool Endpoint::isConnected()
{
  Q_ASSERT(s_instance);
  return s_instance->m_socket;
}

quint16 Endpoint::defaultPort()
{
  return 11732;
}

quint16 Endpoint::broadcastPort()
{
  return 13325;
}

void Endpoint::setDevice(QIODevice* device)
{
  qDebug() << Q_FUNC_INFO << device;
  Q_ASSERT(!m_socket);
  Q_ASSERT(device);
  m_socket = device;
  connect(m_socket.data(), SIGNAL(readyRead()), SLOT(readyRead()));
  connect(m_socket.data(), SIGNAL(disconnected()), SLOT(connectionClosed()));
  if (m_socket->bytesAvailable())
    readyRead();
}

Protocol::ObjectAddress Endpoint::endpointAddress() const
{
  return m_myAddress;
}

void Endpoint::readyRead()
{
  while (Message::canReadMessage(m_socket.data())) {
    messageReceived(Message::readMessage(m_socket.data()));
  }
}

void Endpoint::connectionClosed()
{
  qDebug() << Q_FUNC_INFO;
  m_socket->deleteLater();
  m_socket = 0;
  emit disconnected();
}

Protocol::ObjectAddress Endpoint::objectAddress(const QString& objectName) const
{
  const QHash<QString, ObjectInfo*>::const_iterator it = m_nameMap.constFind(objectName);
  if (it != m_nameMap.constEnd())
    return it.value()->address;
  return Protocol::InvalidObjectAddress;
}

void Endpoint::registerObjectInternal(const QString& objectName, Protocol::ObjectAddress objectAddress)
{
  Q_ASSERT(objectAddress != Protocol::InvalidObjectAddress);
  qDebug() << objectName << objectAddress;

  ObjectInfo *obj = new ObjectInfo;
  obj->address = objectAddress;
  obj->name = objectName;
  insertObjectInfo(obj);

  emit objectRegistered(objectName, objectAddress);
}

void Endpoint::unregisterObjectInternal(const QString& objectName)
{
  Q_ASSERT(m_nameMap.contains(objectName));
  ObjectInfo *obj = m_nameMap.value(objectName);

  emit objectUnregistered(objectName, obj->address);
  removeObjectInfo(obj);
}

void Endpoint::registerMessageHandlerInternal(Protocol::ObjectAddress objectAddress, QObject* receiver, const char* messageHandlerName)
{
  Q_ASSERT(m_addressMap.contains(objectAddress));
  ObjectInfo *obj = m_addressMap.value(objectAddress);
  Q_ASSERT(obj);
  Q_ASSERT(!obj->receiver);
  Q_ASSERT(obj->messageHandler.isEmpty());
  obj->receiver = receiver;
  obj->messageHandler = messageHandlerName;
  Q_ASSERT(!m_handlerMap.contains(receiver, obj));
  m_handlerMap.insert(receiver, obj);
  connect(receiver, SIGNAL(destroyed(QObject*)), SLOT(handlerDestroyed(QObject*)));
}

void Endpoint::unregisterMessageHandlerInternal(Protocol::ObjectAddress objectAddress)
{
  Q_ASSERT(m_addressMap.contains(objectAddress));
  ObjectInfo *obj = m_addressMap.value(objectAddress);
  Q_ASSERT(obj);
  Q_ASSERT(obj->receiver);
  disconnect(obj->receiver, SIGNAL(destroyed(QObject*)), this, SLOT(handlerDestroyed(QObject*)));
  m_handlerMap.remove(obj->receiver, obj);
  obj->receiver = 0;
  obj->messageHandler.clear();
}

void Endpoint::handlerDestroyed(QObject* obj)
{
  const QList<ObjectInfo*> objs = m_handlerMap.values(obj); // copy, the virtual method below likely changes the maps.
  m_handlerMap.remove(obj);
  foreach (ObjectInfo *obj, objs) {
    obj->receiver = 0;
    obj->messageHandler.clear();
    handlerDestroyed(obj->address, QString(obj->name)); // copy the name, in case unregisterMessageHandlerInternal() is called inside
  }
}

void Endpoint::dispatchMessage(const Message& msg)
{
  const QHash<Protocol::ObjectAddress, ObjectInfo*>::const_iterator it = m_addressMap.constFind(msg.address());
  if (it == m_addressMap.constEnd())
    return;

  ObjectInfo* obj = it.value();
  QMetaObject::invokeMethod(obj->receiver, obj->messageHandler, Q_ARG(GammaRay::Message, msg));
}

QVector< QPair< Protocol::ObjectAddress, QString > > Endpoint::objectAddresses() const
{
  QVector<QPair<Protocol::ObjectAddress, QString> > addrs;
  addrs.reserve(m_addressMap.size());
  for (QHash<Protocol::ObjectAddress, ObjectInfo*>::const_iterator it = m_addressMap.constBegin(); it != m_addressMap.constEnd(); ++it)
    addrs.push_back(qMakePair(it.key(), it.value()->name));
  return addrs;
}

void Endpoint::insertObjectInfo(Endpoint::ObjectInfo* oi)
{
  Q_ASSERT(!m_addressMap.contains(oi->address));
  m_addressMap.insert(oi->address, oi);
  Q_ASSERT(!m_nameMap.contains(oi->name));
  m_nameMap.insert(oi->name, oi);

  if (oi->receiver)
    m_handlerMap.insert(oi->receiver, oi);
}

void Endpoint::removeObjectInfo(Endpoint::ObjectInfo* oi)
{
  Q_ASSERT(m_addressMap.contains(oi->address));
  m_addressMap.remove(oi->address);
  Q_ASSERT(m_nameMap.contains(oi->name));
  m_nameMap.remove(oi->name);

  if (oi->receiver)
    m_handlerMap.remove(oi->receiver, oi);

  delete oi;
}

#include "endpoint.moc"
