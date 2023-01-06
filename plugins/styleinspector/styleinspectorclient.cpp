/*
  styleinspectorclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "styleinspectorclient.h"
#include <common/endpoint.h>

using namespace GammaRay;

StyleInspectorClient::StyleInspectorClient(QObject *parent)
    : StyleInspectorInterface(parent)
{
    // make sure the remote server side uses our initial values
    setCellHeight(cellHeight());
    setCellWidth(cellWidth());
    setCellZoom(cellZoom());
}

StyleInspectorClient::~StyleInspectorClient() = default;

void StyleInspectorClient::setCellHeight(int height)
{
    StyleInspectorInterface::setCellHeight(height);
    Endpoint::instance()->invokeObject(objectName(), "setCellHeight", QVariantList() << height);
}

void StyleInspectorClient::setCellWidth(int width)
{
    StyleInspectorInterface::setCellWidth(width);
    Endpoint::instance()->invokeObject(objectName(), "setCellWidth", QVariantList() << width);
}

void StyleInspectorClient::setCellZoom(int zoom)
{
    StyleInspectorInterface::setCellZoom(zoom);
    Endpoint::instance()->invokeObject(objectName(), "setCellZoom", QVariantList() << zoom);
}
