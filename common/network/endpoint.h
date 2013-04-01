/*
  endpoint.h

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

#ifndef GAMMARAY_ENDPOINT_H
#define GAMMARAY_ENDPOINT_H

#include "protocol.h"

#include <QObject>
#include <QPointer>

class QIODevice;
class QDataStream;

namespace GammaRay {

class Message;

/** Network protocol endpoint.
 *  Contains:
 *  - object address <-> object name mapping
 *  - message handler registration and message dispatching
 */
class Endpoint : public QObject
{
  Q_OBJECT
public:
  ~Endpoint();

  /** Send @p msg to the connected endpoint. */
  static void send(const Message &msg);
  /** Returns @c true if we are currently connected to another endpoint. */
  static bool isConnected();

  static quint16 defaultPort();
  static quint16 broadcastPort();

  /** Returns the object address for @p objectName, or @c Protocol::InvalidObjectAddress if not known. */
  Protocol::ObjectAddress objectAddress(const QString &objectName) const;

  /** Singleton accessor. */
  static Endpoint* instance();

signals:
  /** Emitted when we lost the connection to the other endpoint. */
  void disconnected();

  /** Emitted when a new object with name @p objectName has been registered at address @p objectAddress. */
  void objectRegistered(const QString &objectName, Protocol::ObjectAddress objectAddress);
  void objectUnregistered(const QString &objectName, Protocol::ObjectAddress objectAddress);

protected:
  Endpoint(QObject* parent = 0);
  /** Call with the socket once you have established a connection to another endpoint, takes ownership of @p device. */
  void setDevice(QIODevice* device);

  /** The object address of the other endpoint. */
  Protocol::ObjectAddress endpointAddress() const;

  /** Called for every incoming message.
   *  @see dispatchMessage().
   */
  virtual void messageReceived(const Message &msg) = 0;

  /** Call this when learning about a new object <-> address mapping. */
  void registerObjectInternal(const QString &objectName, Protocol::ObjectAddress objectAddress);
  /** Call this when learning about a dissolved object <-> address mapping. */
  void unregisterObjectInternal(const QString& objectName);

  /** Register the slot @p messageHandlerName on @p receiver as the handler for messages to/from @p objectAddress.
   *  @see dispatchMessage()
   */
  void registerMessageHandlerInternal(Protocol::ObjectAddress objectAddress, QObject *receiver, const char* messageHandlerName);

  /** Unregister the message handler for @p objectAddress. */
  void unregisterMessageHandlerInternal(Protocol::ObjectAddress objectAddress);

  /** Called when the current handler of the object identified by @p objectAddress has been destroyed. */
  virtual void handlerDestroyed(Protocol::ObjectAddress objectAddress, const QString &objectName) = 0;

  /** Calls the message handler registerd for the receiver of @p msg. */
  void dispatchMessage(const GammaRay::Message& msg);

  /** All current object name/address pairs. */
  QVector<QPair<Protocol::ObjectAddress, QString> > objectAddresses() const;

  /** Singleton instance. */
  static Endpoint *s_instance;

private slots:
  void readyRead();
  void connectionClosed();
  void handlerDestroyed(QObject* obj);

private:
  struct ObjectInfo {
    ObjectInfo() : receiver(0) {}
    QString name;
    Protocol::ObjectAddress address;
    QObject* receiver;
    QByteArray messageHandler;
  };

  /** Inserts @p oi into all maps. */
  void insertObjectInfo(ObjectInfo *oi);
  /** Removes @p oi from all maps and destroys it. */
  void removeObjectInfo(ObjectInfo *oi);

  QHash<QString, ObjectInfo*> m_nameMap;
  QHash<Protocol::ObjectAddress, ObjectInfo*> m_addressMap;
  QMultiHash<QObject*, ObjectInfo*> m_handlerMap;

  QPointer<QIODevice> m_socket;
  Protocol::ObjectAddress m_myAddress;
};

}

#endif // GAMMARAY_ENDPOINT_H
