/*
  localclientdevice.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_LOCALCLIENTDEVICE_H
#define GAMMARAY_LOCALCLIENTDEVICE_H

#include "clientdevice.h"

#include <QLocalSocket>

namespace GammaRay {
class LocalClientDevice : public ClientDeviceImpl<QLocalSocket>
{
    Q_OBJECT
public:
    explicit LocalClientDevice(QObject *parent = nullptr);
    void connectToHost() override;
    void disconnectFromHost() override;

private slots:
    void socketError();
};
}

#endif // GAMMARAY_LOCALCLIENTDEVICE_H
