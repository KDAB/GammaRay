/*
  client.h

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

#ifndef GAMMARAY_CLIENT_H
#define GAMMARAY_CLIENT_H

#include <network/protocol.h>
#include <network/endpoint.h>

namespace GammaRay {

class NetworkObject;

/** Client-side connection endpoint. */
class Client : public Endpoint
{
  Q_OBJECT
public:
  explicit Client(QObject *parent = 0);
  ~Client();

  /** Connect to @p hostName on port @p port. */
  void connectToHost(const QString &hostName, quint16 port);

  /**
   * Register a client-side NetworkObject to receive messages from the remote side.
   */
  void registerObject(NetworkObject *object);

  /** Register a message handler for @p objectAddress on object @p handler.
   *  Once a message for this object is received, @p slot is called.
   */
  void registerForObject(Protocol::ObjectAddress objectAddress, QObject *handler, const char* slot);

  /** Unregister the message handler for @p objectAddress. */
  void unregisterForObject(Protocol::ObjectAddress objectAddress);

  /** Singleton accessor. */
  static Client* instance();

signals:
  /** Emitted when we successfully established a connection and passed the protocol version handshake step. */
  void connectionEstablished();
  /** Emitted on connection errors. */
  void connectionError(const QString &msg);

protected:
  void messageReceived(const Message& msg);
  void handlerDestroyed(Protocol::ObjectAddress objectAddress, const QString& objectName);

  void connectObjectToServer(NetworkObject *object);

private:
  void unmonitorObject(Protocol::ObjectAddress objectAddress);

private slots:
  void socketConnected();
  void socketError();

  void serverObjectRegistered(const QString& objectName, Protocol::ObjectAddress objectAddress);
  void serverObjectUnregistered(const QString& objectName, Protocol::ObjectAddress objectAddress);

private:
  QHash<QString, NetworkObject*> m_objects;
  bool m_versionChecked;
};

}

#endif // GAMMARAY_CLIENT_H
