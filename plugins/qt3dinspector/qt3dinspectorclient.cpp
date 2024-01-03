/*
  qt3dinspectorclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "qt3dinspectorclient.h"

#include <common/endpoint.h>

using namespace GammaRay;

Qt3DInspectorClient::Qt3DInspectorClient(QObject *parent)
    : Qt3DInspectorInterface(parent)
{
}

Qt3DInspectorClient::~Qt3DInspectorClient()
{
}

void Qt3DInspectorClient::selectEngine(int index)
{
    Endpoint::instance()->invokeObject(objectName(), "selectEngine",
                                       QVariantList() << QVariant::fromValue(index));
}
