/*
  client.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CLIENT_H
#define GAMMARAY_CLIENT_H

#include <common/protocol.h>
#include <common/endpoint.h>

#include <QUrl>

namespace GammaRay {
class ClientDevice;
class MessageStatisticsModel;

/** Client-side connection endpoint. */
class Client : public Endpoint
{
    Q_OBJECT
public:
    explicit Client(QObject *parent = nullptr);
    ~Client() override;

    /** Connect to a server reachable on @p url. */
    void connectToHost(const QUrl &url, int tryAgain = 0);
    void disconnectFromHost();

    /**
     * Register a client-side QObject to send/receive messages to/from the server side.
     */
    Protocol::ObjectAddress registerObject(const QString &name, QObject *object) override;

    /** Singleton accessor. */
    static Client *instance();

    bool isRemoteClient() const override;
    QUrl serverAddress() const override;

    void registerMessageHandler(Protocol::ObjectAddress objectAddress, QObject *receiver,
                                const char *messageHandlerName) override;
    void unregisterMessageHandler(Protocol::ObjectAddress objectAddress) override;

signals:
    /** Emitted on transient connection errors.
     *  That is, on errors it's worth re-trying, e.g. because the target wasn't up yet.
     */
    void transientConnectionError();
    /** Emitted on persistent connection errors.
     *  That is, any error that is not a transient one.
     */
    void persisitentConnectionError(const QString &msg);

protected:
    void messageReceived(const Message &msg) override;
    void objectDestroyed(Protocol::ObjectAddress objectAddress, const QString &objectName,
                         QObject *object) override;
    void handlerDestroyed(Protocol::ObjectAddress objectAddress,
                          const QString &objectName) override;
    void doSendMessage(const GammaRay::Message &msg) override;

private:
    void monitorObject(Protocol::ObjectAddress objectAddress);
    void unmonitorObject(Protocol::ObjectAddress objectAddress);

private slots:
    void socketConnected();
    void resetClientDevice();
    void socketDisconnected();

private:
    enum InitState
    {
        VersionChecked = 0x1,
        ObjectMapReceived = 0x2,
        ServerInfoReceived = 0x4,
        ServerDataVersionNegotiated = 0x8,
        ConnectionEstablished = 0x10,

        InitComplete = VersionChecked | ObjectMapReceived | ServerInfoReceived | ServerDataVersionNegotiated
    };
    QUrl m_serverAddress;
    ClientDevice *m_clientDevice;
    MessageStatisticsModel *m_statModel;
    int m_initState;
};
}

#endif // GAMMARAY_CLIENT_H
