/*
  tcpserverdevice.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
