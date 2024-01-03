/*
  localserverdevice.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_LOCALSERVERDEVICE_H
#define GAMMARAY_LOCALSERVERDEVICE_H

#include "serverdevice.h"

#include <QLocalServer>
#include <QLocalSocket>

namespace GammaRay {
class LocalServerDevice : public ServerDeviceImpl<QLocalServer>
{
    Q_OBJECT
public:
    explicit LocalServerDevice(QObject *parent = nullptr);

    bool listen() override;
    bool isListening() const override;
    QUrl externalAddress() const override;
};
}

#endif // GAMMARAY_LOCALSERVERDEVICE_H
