/*
  connectionsextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
