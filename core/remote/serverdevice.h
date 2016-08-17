/*
  serverdevice.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#ifndef GAMMARAY_SERVERDEVICE_H
#define GAMMARAY_SERVERDEVICE_H

#include <QObject>
#include <QUrl>

QT_BEGIN_NAMESPACE
class QIODevice;
QT_END_NAMESPACE

namespace GammaRay {
/** Abstract base class for the actual transport implementation. */
class ServerDevice : public QObject
{
    Q_OBJECT
public:
    explicit ServerDevice(QObject *parent = 0);
    ~ServerDevice();

    void setServerAddress(const QUrl &serverAddress);

    virtual bool listen() = 0;
    virtual bool isListening() const = 0;
    virtual QString errorString() const = 0;
    virtual QIODevice *nextPendingConnection() = 0;

    /** An externally useable address of this server.
     *  This might be different from @p serverAddress as passed in the constructor.
     */
    virtual QUrl externalAddress() const = 0;

    static ServerDevice *create(const QUrl &serverAddress, QObject *parent = 0);

    /** Broadcast the given message on an appropriate channel, if backend supports broadcasting. */
    virtual void broadcast(const QByteArray &data);

signals:
    void newConnection();

protected:
    QUrl m_address;
};

template<typename ServerT>
class ServerDeviceImpl : public ServerDevice
{
public:
    explicit inline ServerDeviceImpl(QObject *parent = 0)
        : ServerDevice(parent)
        , m_server(0)
    {
    }

    ~ServerDeviceImpl()
    {
    }

    QString errorString() const Q_DECL_OVERRIDE
    {
        return m_server->errorString();
    }

    QIODevice *nextPendingConnection() Q_DECL_OVERRIDE
    {
        Q_ASSERT(m_server->hasPendingConnections());
        return m_server->nextPendingConnection();
    }

protected:
    ServerT *m_server;
};
}

#endif // GAMMARAY_SERVERDEVICE_H
