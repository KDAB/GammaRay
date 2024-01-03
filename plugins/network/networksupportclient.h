/*
  networksupportclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2019 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Shantanu Tushar <shantanu.tushar@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_NETWORKSUPPORTCLIENT_H
#define GAMMARAY_NETWORKSUPPORTCLIENT_H

#include "networksupportinterface.h"

namespace GammaRay {
class NetworkSupportClient : public NetworkSupportInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::NetworkSupportInterface)
public:
    explicit NetworkSupportClient(QObject *parent = nullptr);
    ~NetworkSupportClient() override;
};
}

#endif // GAMMARAY_NETWORKSUPPORTCLIENT_H
