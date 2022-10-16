/*
  connectionsextension.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <config-gammaray.h>

#include "connectionsextension.h"
#include "inboundconnectionsmodel.h"
#include "outboundconnectionsmodel.h"

#include <core/probe.h>
#include <core/propertycontroller.h>

#include <common/tools/objectinspector/connectionsmodelroles.h>

using namespace GammaRay;

ConnectionsExtension::ConnectionsExtension(PropertyController *controller)
    : ConnectionsExtensionInterface(
        controller->objectBaseName() + ".connectionsExtension", controller)
    , PropertyControllerExtension(controller->objectBaseName() + ".connections")
{
    m_inboundModel = new InboundConnectionsModel(controller);
    m_outboundModel = new OutboundConnectionsModel(controller);

    controller->registerModel(m_inboundModel, QStringLiteral("inboundConnections"));
    controller->registerModel(m_outboundModel, QStringLiteral("outboundConnections"));
}

ConnectionsExtension::~ConnectionsExtension() = default;

bool ConnectionsExtension::setQObject(QObject *object)
{
    m_inboundModel->setObject(object);
    m_outboundModel->setObject(object);

    return true;
}

void ConnectionsExtension::navigateToSender(int modelRow)
{
    const QModelIndex index = m_inboundModel->index(modelRow, 0);
    QObject *sender = index.data(ConnectionsModelRoles::EndpointRole).value<QObject *>();
    if (!sender)
        return;
    Probe::instance()->selectObject(sender);
}

void ConnectionsExtension::navigateToReceiver(int modelRow)
{
    const QModelIndex index = m_outboundModel->index(modelRow, 0);
    QObject *receiver = index.data(ConnectionsModelRoles::EndpointRole).value<QObject *>();
    if (!receiver)
        return;
    Probe::instance()->selectObject(receiver);
}
