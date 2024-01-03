/*
  probecontrollerclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROBECONTROLLERCLIENT_H
#define GAMMARAY_PROBECONTROLLERCLIENT_H

#include <common/probecontrollerinterface.h>

namespace GammaRay {
class ProbeControllerClient : public ProbeControllerInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ProbeControllerInterface)
public:
    explicit ProbeControllerClient(QObject *parent = nullptr);

    void detachProbe() override;
    void quitHost() override;
};
}

#endif // GAMMARAY_PROBECONTROLLERCLIENT_H
