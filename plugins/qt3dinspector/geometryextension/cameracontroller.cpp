/*
  cameracontroller.cpp

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

#include "cameracontroller.h"

#include <Qt3DRender/QCamera>

#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAnalogAxisInput>
#include <Qt3DInput/QButtonAxisInput>
#include <Qt3DInput/QAction>
#include <Qt3DInput/QActionInput>
#include <Qt3DInput/QLogicalDevice>
#include <Qt3DInput/QKeyboardDevice>
#include <Qt3DInput/QMouseDevice>
#include <Qt3DInput/QMouseEvent>
#include <Qt3DLogic/QFrameAction>

#include <QDebug>

using namespace GammaRay;

CameraController::CameraController(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , m_camera(nullptr)
    , m_leftMouseButtonAction(new Qt3DInput::QAction(this))
    , m_midMouseButtonAction(new Qt3DInput::QAction(this))
    , m_keyboardDevice(new Qt3DInput::QKeyboardDevice)
{
    for (int i = 0; i < AXISCOUNT; ++i)
        m_axis[i] = new Qt3DInput::QAxis(this);

    // LMB for first person control
    auto mouseDevice = new Qt3DInput::QMouseDevice(this);
    auto leftMouseButtonInput = new Qt3DInput::QActionInput(this);
    leftMouseButtonInput->setButtons({Qt3DInput::QMouseEvent::LeftButton});
    leftMouseButtonInput->setSourceDevice(mouseDevice);
    m_leftMouseButtonAction->addInput(leftMouseButtonInput);

    // MMB for orbit control
    auto midMouseButtonInput = new Qt3DInput::QActionInput(this);
    midMouseButtonInput->setButtons({Qt3DInput::QMouseEvent::MiddleButton});
    midMouseButtonInput->setSourceDevice(mouseDevice);
    m_midMouseButtonAction->addInput(midMouseButtonInput);

    // mouse wheel zoom/strafe
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    auto xWheelInput = new Qt3DInput::QAnalogAxisInput(this);
    xWheelInput->setAxis(Qt3DInput::QMouseDevice::WheelX);
    xWheelInput->setSourceDevice(mouseDevice);
    m_axis[TX]->addInput(xWheelInput);
    auto yWheelInput = new Qt3DInput::QAnalogAxisInput(this);
    yWheelInput->setAxis(Qt3DInput::QMouseDevice::WheelY);
    yWheelInput->setSourceDevice(mouseDevice);
    m_axis[TZ]->addInput(yWheelInput);
#endif

    // TODO: shift for slow motion, ctrl for fast motion

    // X rotation
    auto mouseRxInput = new Qt3DInput::QAnalogAxisInput;
    mouseRxInput->setAxis(Qt3DInput::QMouseDevice::X);
    mouseRxInput->setSourceDevice(mouseDevice);
    m_axis[RX]->addInput(mouseRxInput);

    // Y rotation
    auto mouseRyInput = new Qt3DInput::QAnalogAxisInput;
    mouseRyInput->setAxis(Qt3DInput::QMouseDevice::Y);
    mouseRyInput->setSourceDevice(mouseDevice);
    m_axis[RY]->addInput(mouseRyInput);

    // X translation
    addKeyboardInput(TX, Qt::Key_Right, 1.0f);
    addKeyboardInput(TX, Qt::Key_D, 1.0f);
    addKeyboardInput(TX, Qt::Key_Left, -1.0f);
    addKeyboardInput(TX, Qt::Key_A, -1.0f);

    // Y translation
    addKeyboardInput(TY, Qt::Key_PageUp, 1.0f);
    addKeyboardInput(TY, Qt::Key_R, 1.0f);
    addKeyboardInput(TY, Qt::Key_PageDown, -1.0f);
    addKeyboardInput(TY, Qt::Key_F, -1.0f);

    // Z translation
    addKeyboardInput(TZ, Qt::Key_Up, 1.0f);
    addKeyboardInput(TZ, Qt::Key_W, 1.0f);
    addKeyboardInput(TZ, Qt::Key_Down, -1.0f);
    addKeyboardInput(TZ, Qt::Key_S, -1.0f);

    // logical device
    auto m_logicalDevice = new Qt3DInput::QLogicalDevice;
    m_logicalDevice->addAction(m_leftMouseButtonAction);
    m_logicalDevice->addAction(m_midMouseButtonAction);
    for (int i = 0; i < AXISCOUNT; ++i)
        m_logicalDevice->addAxis(m_axis[i]);
    addComponent(m_logicalDevice);

    auto m_frameAction = new Qt3DLogic::QFrameAction;
    connect(m_frameAction, &Qt3DLogic::QFrameAction::triggered, this,
            &CameraController::frameActionTriggered);
    addComponent(m_frameAction);
}

CameraController::~CameraController()
{
}

void CameraController::setCamera(Qt3DRender::QCamera *camera)
{
    m_camera = camera;
}

void CameraController::frameActionTriggered(float dt)
{
    if (!m_camera)
        return;
    m_camera->translate(QVector3D(m_axis[TX]->value() * m_linearSpeed,
                                  m_axis[TY]->value() * m_linearSpeed,
                                  m_axis[TZ]->value() * m_linearSpeed) * dt);
    if (m_leftMouseButtonAction->isActive()) {
        m_camera->pan(m_axis[RX]->value() * m_lookSpeed * dt, QVector3D(0.0f, 1.0f, 0.0f));
        m_camera->tilt(m_axis[RY]->value() * m_lookSpeed * dt);
    } else if (m_midMouseButtonAction->isActive()) {
        m_camera->panAboutViewCenter(m_axis[RX]->value() * m_lookSpeed * dt, QVector3D(0.0f, 1.0f, 0.0f));
        m_camera->tiltAboutViewCenter(m_axis[RY]->value() * m_lookSpeed * dt);
    }
}

void CameraController::addKeyboardInput(Axis axis, Qt::Key key, float scale)
{
    auto keyboardInput = new Qt3DInput::QButtonAxisInput;
    keyboardInput->setButtons({key});
    keyboardInput->setScale(scale);
    keyboardInput->setSourceDevice(m_keyboardDevice);
    m_axis[axis]->addInput(keyboardInput);
}
