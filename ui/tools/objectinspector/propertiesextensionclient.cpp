/*
  propertiesextensionclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "propertiesextensionclient.h"

#include <common/endpoint.h>
#include <common/metatypedeclarations.h>
#include <common/variantwrapper.h>

using namespace GammaRay;

PropertiesExtensionClient::PropertiesExtensionClient(const QString &name, QObject *parent)
    : PropertiesExtensionInterface(name, parent)
{
}

PropertiesExtensionClient::~PropertiesExtensionClient() = default;

void PropertiesExtensionClient::setProperty(const QString &propertyName, const QVariant &value)
{
    Endpoint::instance()->invokeObject(name(), "setProperty",
                                       QVariantList() << QVariant::fromValue(
                                           propertyName)
                                                      << VariantWrapper(value));
}
