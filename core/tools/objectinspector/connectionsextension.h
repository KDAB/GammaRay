/*
  connectionsextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_OBJECTINSPECTOR_CONNECTIONSEXTENSION_H
#define GAMMARAY_OBJECTINSPECTOR_CONNECTIONSEXTENSION_H

#include "common/tools/objectinspector/connectionsextensioninterface.h"
#include "core/propertycontrollerextension.h"

namespace GammaRay {
class InboundConnectionsModel;
class OutboundConnectionsModel;

class ConnectionsExtension : public ConnectionsExtensionInterface,
                             public PropertyControllerExtension
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ConnectionsExtensionInterface)
public:
    explicit ConnectionsExtension(PropertyController *controller);
    ~ConnectionsExtension() override;

    bool setQObject(QObject *object) override;

public slots:
    void navigateToReceiver(int modelRow) override;
    void navigateToSender(int modelRow) override;

private:
    InboundConnectionsModel *m_inboundModel;
    OutboundConnectionsModel *m_outboundModel;
};
}

#endif // GAMMARAY_CONNECTIONSEXTENSION_H
