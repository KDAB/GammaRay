/*
  widget3dimagetextureimage.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

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

#ifndef WIDGET3DCAMERACONTROLLER
#define WIDGET3DCAMERACONTROLLER

#include <Qt3DCore/QEntity>
#include <Qt3DRender/QCamera>
#include <QVector3D>

QT_BEGIN_NAMESPACE

namespace Qt3DInput
{
class QKeyboardDevice;
class QMouseDevice;
class QLogicalDevice;
class QAction;
class QAxis;
}
namespace Qt3DLogic
{
class QFrameAction;
}

QT_END_NAMESPACE

class Widget3DCameraController : public Qt3DCore::QEntity
{
    Q_OBJECT

    Q_PROPERTY(Qt3DRender::QCamera* camera READ camera WRITE setCamera)
public:
    explicit Widget3DCameraController(Qt3DCore::QNode *parent = Q_NULLPTR);
    ~Widget3DCameraController();

    void setCamera(Qt3DRender::QCamera *camera);
    Qt3DRender::QCamera *camera() const;

private Q_SLOTS:
    void frameActionTriggered(float dt);

private:
    Qt3DRender::QCamera *mCamera;
    QVector3D mUpVector;

    Qt3DInput::QKeyboardDevice *mKeyboardDevice;
    Qt3DInput::QMouseDevice *mMouseDevice;
    Qt3DInput::QLogicalDevice *mLogicalDevice;

    Qt3DInput::QAction *mMoveAction;
    Qt3DInput::QAction *mRotateAction;
    Qt3DInput::QAction *mZoomInAction;
    Qt3DInput::QAction *mZoomOutAction;
    Qt3DInput::QAction *mRotateXAction;
    Qt3DInput::QAction *mRotateYAction;

    Qt3DInput::QAxis *mRotateXAxis;
    Qt3DInput::QAxis *mRotateYAxis;
    Qt3DInput::QAxis *mTranslateXAxis;
    Qt3DInput::QAxis *mTranslateYAxis;

    Qt3DLogic::QFrameAction *mFrameAction;
};

#endif
