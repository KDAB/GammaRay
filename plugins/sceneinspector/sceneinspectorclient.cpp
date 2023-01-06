/*
  sceneinspectorclient.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
