/*
  server.h

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

#ifndef GAMMARAY_SERVER_H
#define GAMMARAY_SERVER_H

#include <common/endpoint.h>
#include <common/objectbroker.h>

class QTcpServer;
class QUdpSocket;
class QTimer;
class QSignalSpy;

namespace GammaRay {

/** Server side connection endpoint. */
class Server : public Endpoint
{
  Q_OBJECT
  public:
    explicit Server(QObject *parent = 0);
    ~Server();

    /**
     * Register a server-side QObject to send/receive messages to/from the client side.
     */
    virtual Protocol::ObjectAddress registerObject(const QString &name, QObject *object);

    /** Register a new object with name @p objectName as a destination for messages.
     *  New messages to that object are passed to the slot @p messageHandlerName on @p receiver.
     *  If the object is unused on the client side it might be useful to disable sending out signals or
     *  other expensive operations, when this state changes the slot @p monitorNotifier is called.
     */
    Protocol::ObjectAddress registerObject(const QString &objectName, QObject* receiver, const char* messageHandlerName, const char* monitorNotifier = 0);

    /** Singleton accessor. */
    static Server* instance();

    /**
     * Call @p method on the remote client and also directly on the local object identified by @p objectName.
     */
    virtual void invokeObject(const QString &objectName, const char *method, const QVariantList &args = QVariantList()) const;

    bool isRemoteClient() const;
    quint16 port() const;
  protected:
    void messageReceived(const Message& msg);
    void handlerDestroyed(Protocol::ObjectAddress objectAddress, const QString& objectName);
    void objectDestroyed(Protocol::ObjectAddress objectAddress, const QString &objectName, QObject *object);

  private slots:
    void newConnection();
    void broadcast();

    /**
     * Forward the signal that triggered the call to this slot to the remote client if connected.
     */
    void forwardSignal() const;

  private:
    QTcpServer *m_tcpServer;
    QHash<Protocol::ObjectAddress, QPair<QObject*, QByteArray> > m_monitorNotifiers;
    Protocol::ObjectAddress m_nextAddress;

    QString m_label;
    QTimer* m_broadcastTimer;
    QUdpSocket* m_broadcastSocket;

    // maps registered objects to a map of signal index to signal spy
    QHash<QObject*, QHash<int, QSignalSpy*> > m_signalForwards;
};

}

#endif
