/*
  connectionsextensionclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
