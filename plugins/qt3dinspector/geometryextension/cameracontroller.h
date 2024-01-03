/*
  cameracontroller.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_CAMERACONTROLLER_H
#define GAMMARAY_CAMERACONTROLLER_H

#include <Qt3DCore/QEntity>

QT_BEGIN_NAMESPACE
namespace Qt3DRender {
class QCamera;
}
namespace Qt3DInput {
class QAction;
class QAxis;
class QKeyboardDevice;
}
QT_END_NAMESPACE

namespace GammaRay {
class CameraController : public Qt3DCore::QEntity
{
    Q_OBJECT
public:
    explicit CameraController(Qt3DCore::QNode *parent = nullptr);
    ~CameraController();

    void setCamera(Qt3DRender::QCamera *camera);

private:
    enum Axis
    {
        RX,
        RY,
        RZ,
        TX,
        TY,
        TZ,
        AXISCOUNT
    };

    void frameActionTriggered(float dt);
    void addKeyboardInput(Axis axis, Qt::Key key, float scale);

    Qt3DRender::QCamera *m_camera;

    Qt3DInput::QAction *m_leftMouseButtonAction;
    Qt3DInput::QAction *m_midMouseButtonAction;

    Qt3DInput::QAxis *m_axis[AXISCOUNT];

    Qt3DInput::QKeyboardDevice *m_keyboardDevice;

    float m_linearSpeed = 10.0f; // TODO
    float m_lookSpeed = 360.0f; // TODO
};
}

#endif // GAMMARAY_CAMERACONTROLLER_H
