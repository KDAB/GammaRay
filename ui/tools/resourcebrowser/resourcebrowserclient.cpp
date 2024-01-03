/*
  resourcebrowserclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
