/*
  connectionsextensionclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CONNECTIONSEXTENSIONCLIENT_H
#define GAMMARAY_CONNECTIONSEXTENSIONCLIENT_H

#include <common/tools/objectinspector/connectionsextensioninterface.h>

namespace GammaRay {
class ConnectionsExtensionClient : public ConnectionsExtensionInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ConnectionsExtensionInterface)
public:
    explicit ConnectionsExtensionClient(const QString &name, QObject *parent = nullptr);
    ~ConnectionsExtensionClient() override;

public slots:
    void navigateToReceiver(int modelRow) override;
    void navigateToSender(int modelRow) override;
};
}

#endif // GAMMARAY_CONNECTIONSEXTENSIONCLIENT_H
