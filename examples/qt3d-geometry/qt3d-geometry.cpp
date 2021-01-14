/*
  qt3d-geometry.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "mycylinder.h"

#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DExtras/QOrbitCameraController>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCameraLens>

#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>

#include <QGuiApplication>
#include <QPropertyAnimation>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);
    Qt3DExtras::Qt3DWindow view;

    auto rootEntity = new Qt3DCore::QEntity;
    rootEntity->setObjectName("rootEntity");

    auto cylinderEntity = new Qt3DCore::QEntity(rootEntity);
    cylinderEntity->setObjectName("cylinderEntity");

    auto mesh = new MyCylinder;

    auto material = new Qt3DExtras::QPhongMaterial(rootEntity);
    material->setAmbient(QColor(0, 85, 128));
    material->setDiffuse(QColor(0, 128, 192));
    material->setSpecular(QColor(0, 170, 255));

    auto transform = new Qt3DCore::QTransform;
    transform->setRotation(QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), 45.0f));
    auto animation = new QPropertyAnimation(transform);
    animation->setTargetObject(transform);
    animation->setPropertyName("rotationZ");
    animation->setStartValue(QVariant::fromValue(0));
    animation->setEndValue(QVariant::fromValue(360));
    animation->setDuration(10000);
    animation->setLoopCount(-1);
    animation->start();

    cylinderEntity->addComponent(mesh);
    cylinderEntity->addComponent(transform);
    cylinderEntity->addComponent(material);

    // Camera
    auto camera = view.camera();
    camera->setObjectName("camera");
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 6.0f));
    camera->setViewCenter(QVector3D(0, 0, 0));

    auto camController = new Qt3DExtras::QOrbitCameraController(rootEntity);
    camController->setLinearSpeed( 50.0f );
    camController->setLookSpeed( 180.0f );
    camController->setCamera(camera);

    view.setRootEntity(rootEntity);
    view.show();

    return app.exec();
}
