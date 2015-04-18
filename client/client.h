/*
  client.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <common/protocol.h>
#include <common/endpoint.h>

#include <QUrl>

namespace GammaRay {

class ClientDevice;

/** Client-side connection endpoint. */
class Client : public Endpoint
{
  Q_OBJECT
public:
  explicit Client(QObject *parent = 0);
  ~Client();

  /** Connect to a server reachable on @p url. */
  void connectToHost(const QUrl &url);
  void disconnectFromHost();

  /**
   * Register a client-side QObject to send/receive messages to/from the server side.
   */
  Protocol::ObjectAddress registerObject(const QString &name, QObject *object);

  /** Register a message handler for @p objectAddress on object @p handler.
   *  Once a message for this object is received, @p slot is called.
   *
   * TODO: get rid of this
   */
  void registerForObject(Protocol::ObjectAddress objectAddress, QObject *handler, const char* slot);

  /** Unregister the message handler for @p objectAddress. */
  void unregisterForObject(Protocol::ObjectAddress objectAddress);

  /** Singleton accessor. */
  static Client* instance();

  bool isRemoteClient() const;
  QUrl serverAddress() const Q_DECL_OVERRIDE;

signals:
  /** Emitted when we successfully established a connection and passed the protocol version handshake step. */
  void connectionEstablished();
  /** Emitted on transient connection errors.
   *  That is, on errors it's worth re-trying, e.g. because the target wasn't up yet.
   */
  void transientConnectionError();
  /** Emitted on persistent connection errors.
   *  That is, any error that is not a transient one.
   */
  void persisitentConnectionError(const QString &msg);

protected:
  void messageReceived(const Message& msg);
  void objectDestroyed(Protocol::ObjectAddress objectAddress, const QString &objectName, QObject *object);
  void handlerDestroyed(Protocol::ObjectAddress objectAddress, const QString& objectName);

private:
  void unmonitorObject(Protocol::ObjectAddress objectAddress);

private slots:
  void socketConnected();
  void socketError();
  void socketDisconnected();

private:
  enum InitState {
    VersionChecked    =  1,
    ObjectMapReceived =  2,
    ServerInfoReceived = 4,
    ConnectionEstablished = 8,

    InitComplete = VersionChecked | ObjectMapReceived | ServerInfoReceived
  };
  QUrl m_serverAddress;
  ClientDevice *m_clientDevice;
  int m_initState;
};

}

#endif // GAMMARAY_CLIENT_H
