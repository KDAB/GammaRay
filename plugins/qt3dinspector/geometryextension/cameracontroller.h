/*
  cameracontroller.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
    enum Axis {
        RX, RY, RZ, TX, TY, TZ, AXISCOUNT
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
