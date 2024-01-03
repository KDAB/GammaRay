/*
  signalmonitorclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SIGNALMONITORCLIENT_H
#define GAMMARAY_SIGNALMONITORCLIENT_H

#include "signalmonitorinterface.h"

namespace GammaRay {
class SignalMonitorClient : public SignalMonitorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::SignalMonitorInterface)
public:
    explicit SignalMonitorClient(QObject *parent = nullptr);
    ~SignalMonitorClient() override;

public slots:
    void sendClockUpdates(bool enabled) override;
};
}

#endif // GAMMARAY_SIGNALMONITORCLIENT_H
