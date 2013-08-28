/*
  networkobject.cpp

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

#include "networkobject.h"
#include "message.h"
#include "endpoint.h"
#include "objectbroker.h"
#include "methodargument.h"

using namespace GammaRay;

NetworkObject::NetworkObject(const QString& objectName, QObject* parent)
  : QObject(parent)
  , m_myAddress(Protocol::InvalidObjectAddress)
{
  Q_ASSERT(!objectName.isEmpty());
  setObjectName(objectName);

  ObjectBroker::registerObject(this);
}

NetworkObject::~NetworkObject()
{
}

Protocol::ObjectAddress NetworkObject::address() const
{
  return m_myAddress;
}

void NetworkObject::setAddress(Protocol::ObjectAddress address)
{
  m_myAddress = address;
}

void NetworkObject::newMessage(const Message& msg)
{
  Q_ASSERT(msg.type() == Protocol::MethodCall);

  QByteArray signalName;
  msg.payload() >> signalName;
  Q_ASSERT(!signalName.isEmpty());

  QVariantList args;
  msg.payload() >> args;

  emitLocal(signalName, args);
}

void NetworkObject::emitSignal(const char *signalName, const QVariantList &args)
{
  const QByteArray name(signalName);
  Q_ASSERT(!name.isEmpty());

  if (Endpoint::isConnected()) {
    Message msg(m_myAddress, Protocol::MethodCall);
    msg.payload() << name << args;
    Endpoint::send(msg);
  }
}

void NetworkObject::subscribeToSignal(const char *signalName, QObject* receiver, const char* slot)
{
  m_subscriptions.insert(signalName, qMakePair(receiver, QByteArray(slot)));
}

void NetworkObject::emitLocal(const char* signalName, const QVariantList& args)
{
  QByteArray name(signalName);
  QObject* receiver = this;
  const QHash<QByteArray, QPair<QObject*, QByteArray> >::const_iterator it = m_subscriptions.constFind(signalName);
  if (it != m_subscriptions.constEnd()) {
    name = it.value().second;
    receiver = it.value().first;
  }

  Q_ASSERT(args.size() <= 10);
  QVector<MethodArgument> a;
  a.reserve(10);
  foreach (const QVariant &v, args)
    a.push_back(MethodArgument(v));
  a.resize(10);

  QMetaObject::invokeMethod(receiver, name, a[0], a[1], a[2], a[3], a[4], a[5], a[6], a[7], a[8], a[9]);
}

#include "networkobject.moc"
