/*
  methodsextensionclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "methodsextensionclient.h"

#include <common/endpoint.h>
#include <common/metatypedeclarations.h>
#include <common/variantwrapper.h>

using namespace GammaRay;

MethodsExtensionClient::MethodsExtensionClient(const QString &name, QObject *parent)
    : MethodsExtensionInterface(name, parent)
{
}

MethodsExtensionClient::~MethodsExtensionClient() = default;

void MethodsExtensionClient::activateMethod()
{
    Endpoint::instance()->invokeObject(name(), "activateMethod");
}

void MethodsExtensionClient::invokeMethod(Qt::ConnectionType type)
{
    Endpoint::instance()->invokeObject(name(), "invokeMethod",
                                       QVariantList() << QVariant::fromValue(type));
}

void MethodsExtensionClient::connectToSignal()
{
    Endpoint::instance()->invokeObject(name(), "connectToSignal");
}
