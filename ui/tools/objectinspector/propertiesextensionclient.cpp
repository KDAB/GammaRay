/*
  propertiesextensionclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
