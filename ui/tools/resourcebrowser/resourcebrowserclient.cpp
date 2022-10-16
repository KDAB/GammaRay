/*
  resourcebrowserclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "resourcebrowserclient.h"

#include <common/endpoint.h>

using namespace GammaRay;

ResourceBrowserClient::ResourceBrowserClient(QObject *parent)
    : ResourceBrowserInterface(parent)
{
}

ResourceBrowserClient::~ResourceBrowserClient() = default;

void ResourceBrowserClient::downloadResource(const QString &sourceFilePath,
                                             const QString &targetFilePath)
{
    Endpoint::instance()->invokeObject(objectName(), "downloadResource",
                                       QVariantList() << sourceFilePath << targetFilePath);
}

void ResourceBrowserClient::selectResource(const QString &sourceFilePath, int line, int column)
{
    Endpoint::instance()->invokeObject(objectName(), "selectResource",
                                       QVariantList() << sourceFilePath << line << column);
}
