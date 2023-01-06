/*
  serverdevice.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    explicit ServerDevice(QObject *parent = nullptr);
    ~ServerDevice() override;

    void setServerAddress(const QUrl &serverAddress);

    virtual bool listen() = 0;
    virtual bool isListening() const = 0;
    virtual QString errorString() const = 0;
    virtual QIODevice *nextPendingConnection() = 0;

    /** An externally usable address of this server.
     *  This might be different from @p serverAddress as passed in the constructor.
     */
    virtual QUrl externalAddress() const = 0;

    static ServerDevice *create(const QUrl &serverAddress, QObject *parent = nullptr);

    /** Broadcast the given message on an appropriate channel, if backend supports broadcasting. */
    virtual void broadcast(const QByteArray &data);

signals:
    void newConnection();
    void externalAddressChanged();

protected:
    QUrl m_address;
};

template<typename ServerT>
class ServerDeviceImpl : public ServerDevice
{
public:
    explicit inline ServerDeviceImpl(QObject *parent = nullptr)
        : ServerDevice(parent)
        , m_server(nullptr)
    {
    }

    ~ServerDeviceImpl() override = default;

    QString errorString() const override
    {
        return m_server->errorString();
    }

    QIODevice *nextPendingConnection() override
    {
        Q_ASSERT(m_server->hasPendingConnections());
        return m_server->nextPendingConnection();
    }

protected:
    ServerT *m_server;
};
}

#endif // GAMMARAY_SERVERDEVICE_H
