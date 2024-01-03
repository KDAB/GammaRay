/*
  toolmanagerclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "toolmanagerclient.h"

#include <common/endpoint.h>

using namespace GammaRay;

ToolManagerClient::ToolManagerClient(QObject *parent)
    : ToolManagerInterface(parent)
{
}

void ToolManagerClient::selectObject(const ObjectId &id, const QString &toolId)
{
    Endpoint::instance()->invokeObject(objectName(), "selectObject",
                                       QVariantList() << QVariant::fromValue(id) << toolId);
}

void ToolManagerClient::requestToolsForObject(const ObjectId &id)
{
    Endpoint::instance()->invokeObject(objectName(), "requestToolsForObject",
                                       QVariantList() << QVariant::fromValue(id));
}

void ToolManagerClient::requestAvailableTools()
{
    Endpoint::instance()->invokeObject(objectName(), "requestAvailableTools");
}
