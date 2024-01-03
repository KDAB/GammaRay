/*
  widgetinspectorclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "widgetinspectorclient.h"
#include <common/endpoint.h>

using namespace GammaRay;

WidgetInspectorClient::WidgetInspectorClient(QObject *parent)
    : WidgetInspectorInterface(parent)
{
}

WidgetInspectorClient::~WidgetInspectorClient() = default;

#define WRAP_REMOTE(func, type)                                                         \
    void WidgetInspectorClient::func(type arg)                                          \
    {                                                                                   \
        Endpoint::instance()->invokeObject(objectName(), #func, QVariantList() << arg); \
    }

WRAP_REMOTE(saveAsImage, const QString &)
WRAP_REMOTE(saveAsSvg, const QString &)
WRAP_REMOTE(saveAsUiFile, const QString &)

void WidgetInspectorClient::analyzePainting()
{
    Endpoint::instance()->invokeObject(objectName(), "analyzePainting");
}
