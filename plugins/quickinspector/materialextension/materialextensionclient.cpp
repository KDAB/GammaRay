/*
  materialextensionclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "materialextensionclient.h"

#include <common/endpoint.h>

using namespace GammaRay;

MaterialExtensionClient::MaterialExtensionClient(const QString &name, QObject *parent)
    : MaterialExtensionInterface(name, parent)
{
}

MaterialExtensionClient::~MaterialExtensionClient() = default;

void MaterialExtensionClient::getShader(int row)
{
    Endpoint::instance()->invokeObject(
        name(), "getShader", QVariantList() << QVariant::fromValue(row));
}
