/*
  qt3dgeometrytab.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "qt3dgeometrytab.h"
#include "ui_qt3dgeometrytab.h"
#include "qt3dgeometryextensioninterface.h"

#include <ui/propertywidget.h>
#include <common/objectbroker.h>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QForwardRenderer>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QSphereMesh>
#include <Qt3DRender/QTechnique>

#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QEntity>

#include <QUrl>
#include <QWindow>

using namespace GammaRay;

Qt3DGeometryTab::Qt3DGeometryTab(PropertyWidget* parent) :
    QWidget(parent),
    ui(new Ui::Qt3DGeometryTab),
    m_aspectEngine(nullptr)
{
    ui->setupUi(this);

    m_interface = ObjectBroker::object<Qt3DGeometryExtensionInterface*>(parent->objectBaseName() + ".qt3dGeometry");
}

Qt3DGeometryTab::~Qt3DGeometryTab()
{
}

void Qt3DGeometryTab::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (m_aspectEngine)
        return;

    auto window = new QWindow;
    window->setSurfaceType(QSurface::OpenGLSurface);
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setSamples(4); // ???
    format.setStencilBufferSize(8); // ???
    window->setFormat(format);
    window->create();

    m_aspectEngine = new Qt3DCore::QAspectEngine(this);
    m_aspectEngine->registerAspect(new Qt3DRender::QRenderAspect);

    // Root entity
    auto rootEntity = new Qt3DCore::QEntity;

    auto camera = new Qt3DRender::QCamera; // TODO automatic aspect ratio, or different projection?
    camera->lens()->setPerspectiveProjection(45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    camera->setPosition(QVector3D(0, 0, 4.0f));

    auto forwardRenderer = new Qt3DRender::QForwardRenderer;
    forwardRenderer->setClearColor(Qt::black);
    forwardRenderer->setCamera(camera);
    forwardRenderer->setSurface(window);

    auto renderSettings = new Qt3DRender::QRenderSettings;
    renderSettings->setActiveFrameGraph(forwardRenderer);
    rootEntity->addComponent(renderSettings);

    // Geometry
    // TODO use the actual geometry
    auto sphereEntity = new Qt3DCore::QEntity(rootEntity);
    auto sphereMesh = new Qt3DRender::QSphereMesh;
    sphereMesh->setRadius(1);

    sphereEntity->addComponent(sphereMesh);
    sphereEntity->addComponent(createMaterial(rootEntity));

    // TODO input handling

    m_aspectEngine->setRootEntity(Qt3DCore::QEntityPtr(rootEntity));

    layout()->addWidget(QWidget::createWindowContainer(window, this));
}

Qt3DCore::QComponent* Qt3DGeometryTab::createMaterial(Qt3DCore::QNode *parent) const
{
    auto material = new Qt3DRender::QMaterial(parent);

    auto shader = new Qt3DRender::QShaderProgram;
    shader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/wireframe.vert"))));
    shader->setGeometryShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/wireframe.geom"))));
    shader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/wireframe.frag"))));

    auto renderPass = new Qt3DRender::QRenderPass;
    renderPass->setShaderProgram(shader);

    auto filterKey = new Qt3DRender::QFilterKey(material);
    filterKey->setName(QStringLiteral("renderingStyle"));
    filterKey->setValue(QStringLiteral("forward"));

    auto technique = new Qt3DRender::QTechnique;
    technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(3);
    technique->graphicsApiFilter()->setMinorVersion(3);
    technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);
    technique->addRenderPass(renderPass);
    technique->addFilterKey(filterKey);

    auto effect = new Qt3DRender::QEffect;
    effect->addTechnique(technique);

    material->setEffect(effect);
    return material;
}

