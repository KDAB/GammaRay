/*
  networksupport.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_NETWORKSUPPORT_H
#define GAMMARAY_NETWORKSUPPORT_H

#include "networksupportinterface.h"

#include <core/toolfactory.h>

namespace GammaRay {
class NetworkSupport : public NetworkSupportInterface
{
    Q_OBJECT
public:
    explicit NetworkSupport(Probe *probe, QObject *parent = nullptr);
    ~NetworkSupport() override;

private:
    static void registerMetaTypes();
    static void registerVariantHandler();
};

class NetworkSupportFactory : public QObject, public StandardToolFactory<QObject, NetworkSupport>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_network.json")

public:
    explicit NetworkSupportFactory(QObject *parent = nullptr);
};
}

#endif // GAMMARAY_NETWORKSUPPORT_H
