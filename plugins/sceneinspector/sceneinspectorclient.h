/*
  sceneinspectorclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SCENEINSPECTOR_SCENEINSPECTORCLIENT_H
#define GAMMARAY_SCENEINSPECTOR_SCENEINSPECTORCLIENT_H

#include "sceneinspectorinterface.h"

namespace GammaRay {
class SceneInspectorClient : public SceneInspectorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::SceneInspectorInterface)
public:
    explicit SceneInspectorClient(QObject *parent = nullptr);
    ~SceneInspectorClient() override;

    void initializeGui() override;
    void renderScene(const QTransform &transform, const QSize &size) override;
    void sceneClicked(const QPointF &pos) override;
};
}

#endif // GAMMARAY_SCENEINSPECTORCLIENT_H
