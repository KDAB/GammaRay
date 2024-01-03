/*
  sceneinspectorclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "sceneinspectorclient.h"

#include <QTransform>

#include <common/endpoint.h>

using namespace GammaRay;

SceneInspectorClient::SceneInspectorClient(QObject *parent)
    : SceneInspectorInterface(parent)
{
}

SceneInspectorClient::~SceneInspectorClient() = default;

void SceneInspectorClient::initializeGui()
{
    Endpoint::instance()->invokeObject(objectName(), "initializeGui");
}

void SceneInspectorClient::renderScene(const QTransform &transform, const QSize &size)
{
    Endpoint::instance()->invokeObject(objectName(), "renderScene",
                                       QVariantList() << transform << size);
}

void SceneInspectorClient::sceneClicked(const QPointF &pos)
{
    Endpoint::instance()->invokeObject(objectName(), "sceneClicked", QVariantList() << pos);
}
