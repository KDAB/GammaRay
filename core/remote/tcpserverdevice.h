/*
  tcpserverdevice.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_TCPSERVERDEVICE_H
#define GAMMARAY_TCPSERVERDEVICE_H

#include "serverdevice.h"

#include <QTcpServer>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
class QUdpSocket;
QT_END_NAMESPACE

namespace GammaRay {
class TcpServerDevice : public ServerDeviceImpl<QTcpServer>
{
    Q_OBJECT
public:
    explicit TcpServerDevice(QObject *parent = nullptr);
    ~TcpServerDevice() override;

    bool listen() override;
    bool isListening() const override;
    QUrl externalAddress() const override;
    void broadcast(const QByteArray &data) override;

private:
    QUdpSocket *m_broadcastSocket;
    QString bestAvailableIP(const QHostAddress &address) const;
};
}

#endif // GAMMARAY_TCPSERVERDEVICE_H
