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
#include "cameracontroller.h"

#include <ui/propertywidget.h>
#include <common/objectbroker.h>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QForwardRenderer>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QRenderAspect>
#include <Qt3DRender/QRenderPass>
#include <Qt3DRender/QRenderSettings>
#include <Qt3DRender/QShaderProgram>
#include <Qt3DRender/QTechnique>

#include <Qt3DInput/QInputAspect>
#include <Qt3DInput/QInputSettings>

#include <Qt3DLogic/QLogicAspect>

#include <Qt3DCore/QAspectEngine>
#include <Qt3DCore/QEntity>

#include <QDebug>
#include <QUrl>
#include <QWindow>

using namespace GammaRay;

Qt3DGeometryTab::Qt3DGeometryTab(PropertyWidget* parent) :
    QWidget(parent),
    ui(new Ui::Qt3DGeometryTab),
    m_surface(nullptr),
    m_aspectEngine(nullptr),
    m_camera(nullptr),
    m_geometryRenderer(nullptr),
    m_normalsRenderPass(nullptr)
{
    ui->setupUi(this);
    connect(ui->resetCam, &QPushButton::clicked, this, &Qt3DGeometryTab::resetCamera);
    connect(ui->showNormals, &QCheckBox::toggled, this, [this]() {
        if (m_normalsRenderPass)
            m_normalsRenderPass->setEnabled(ui->showNormals->isChecked());
    });

    m_interface = ObjectBroker::object<Qt3DGeometryExtensionInterface*>(parent->objectBaseName() + ".qt3dGeometry");
    connect(m_interface, &Qt3DGeometryExtensionInterface::geometryDataChanged, this, &Qt3DGeometryTab::updateGeometry);
}

Qt3DGeometryTab::~Qt3DGeometryTab()
{
}

void Qt3DGeometryTab::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);

    if (m_aspectEngine)
        return;

    m_surface = new QWindow;
    m_surface->setSurfaceType(QSurface::OpenGLSurface);
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setSamples(4); // ???
    format.setStencilBufferSize(8); // ???
    m_surface->setFormat(format);
    m_surface->create();

    m_aspectEngine = new Qt3DCore::QAspectEngine(this);
    m_aspectEngine->registerAspect(new Qt3DRender::QRenderAspect);

    auto rootEntity = new Qt3DCore::QEntity;

    m_camera = new Qt3DRender::QCamera;
    resetCamera();

    // rendering
    auto forwardRenderer = new Qt3DRender::QForwardRenderer;
    forwardRenderer->setClearColor(Qt::black);
    forwardRenderer->setCamera(m_camera);
    forwardRenderer->setSurface(m_surface);

    auto renderSettings = new Qt3DRender::QRenderSettings;
    renderSettings->setActiveFrameGraph(forwardRenderer);
    rootEntity->addComponent(renderSettings);

    auto geometryEntity = new Qt3DCore::QEntity(rootEntity);
    m_geometryRenderer = new Qt3DRender::QGeometryRenderer;
    geometryEntity->addComponent(m_geometryRenderer);
    geometryEntity->addComponent(createMaterial(rootEntity));
    updateGeometry();

    // input handling
    m_aspectEngine->registerAspect(new Qt3DLogic::QLogicAspect);
    m_aspectEngine->registerAspect(new Qt3DInput::QInputAspect);
    auto inputSettings = new Qt3DInput::QInputSettings;
    inputSettings->setEventSource(m_surface);
    rootEntity->addComponent(inputSettings);

    auto camController = new CameraController(rootEntity);
    camController->setCamera(m_camera);

    m_aspectEngine->setRootEntity(Qt3DCore::QEntityPtr(rootEntity));

    layout()->addWidget(QWidget::createWindowContainer(m_surface, this));
}

Qt3DCore::QComponent* Qt3DGeometryTab::createMaterial(Qt3DCore::QNode *parent)
{
    auto material = new Qt3DRender::QMaterial(parent);

    auto wireframeShader = new Qt3DRender::QShaderProgram;
    wireframeShader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/passthrough.vert"))));
    wireframeShader->setGeometryShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/wireframe.geom"))));
    wireframeShader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/wireframe.frag"))));

    auto wireframeRenderPass = new Qt3DRender::QRenderPass;
    wireframeRenderPass->setShaderProgram(wireframeShader);

    auto normalsShader = new Qt3DRender::QShaderProgram;
    normalsShader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/passthrough.vert"))));
    normalsShader->setGeometryShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/normals.geom"))));
    normalsShader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/normals.frag"))));

    m_normalsRenderPass = new Qt3DRender::QRenderPass;
    m_normalsRenderPass->setShaderProgram(normalsShader);
    m_normalsRenderPass->setEnabled(ui->showNormals->isChecked());

    auto filterKey = new Qt3DRender::QFilterKey(material);
    filterKey->setName(QStringLiteral("renderingStyle"));
    filterKey->setValue(QStringLiteral("forward"));

    auto technique = new Qt3DRender::QTechnique;
    technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    technique->graphicsApiFilter()->setMajorVersion(3);
    technique->graphicsApiFilter()->setMinorVersion(3);
    technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);
    technique->addRenderPass(wireframeRenderPass);
    technique->addRenderPass(m_normalsRenderPass);
    technique->addFilterKey(filterKey);

    auto effect = new Qt3DRender::QEffect;
    effect->addTechnique(technique);

    material->setEffect(effect);
    return material;
}

static void setupAttribute(Qt3DRender::QAttribute *attr, const Qt3DGeometryAttributeData &attrData)
{
    attr->setByteOffset(attrData.byteOffset);
    attr->setByteStride(attrData.byteStride);
    attr->setCount(attrData.count);
    attr->setDivisor(attrData.divisor);
    attr->setDataType(attrData.vertexBaseType);
    attr->setDataSize(attrData.vertexSize);
}

void Qt3DGeometryTab::updateGeometry()
{
    if (!m_geometryRenderer)
        return;

    const auto geo = m_interface->geometryData();

    auto geometry = new Qt3DRender::QGeometry(m_geometryRenderer);

    if (!geo.vertexPositions.data.isEmpty()) {
        auto posBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, geometry);
        posBuffer->setData(geo.vertexPositions.data);
        auto posAttr = new Qt3DRender::QAttribute();
        posAttr->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        posAttr->setBuffer(posBuffer);
        setupAttribute(posAttr, geo.vertexPositions);
        posAttr->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
        geometry->addAttribute(posAttr);
    }

    if (!geo.vertexNormals.data.isEmpty()) {
        auto normalBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, geometry);
        normalBuffer->setData(geo.vertexNormals.data);
        auto normalAttr = new Qt3DRender::QAttribute();
        normalAttr->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        normalAttr->setBuffer(normalBuffer);
        setupAttribute(normalAttr, geo.vertexNormals);
        normalAttr->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());
        geometry->addAttribute(normalAttr);
    }

    if (!geo.index.data.isEmpty()) {
        auto indexBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, geometry);
        indexBuffer->setData(geo.index.data);
        auto indexAttr = new Qt3DRender::QAttribute();
        indexAttr->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
        indexAttr->setBuffer(indexBuffer);
        setupAttribute(indexAttr, geo.index);
        geometry->addAttribute(indexAttr);
    }

    m_geometryRenderer->setInstanceCount(1);
    m_geometryRenderer->setIndexOffset(0);
    m_geometryRenderer->setFirstInstance(0);
    m_geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    m_geometryRenderer->setGeometry(geometry);
}

void Qt3DGeometryTab::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (m_surface && m_camera)
        m_camera->lens()->setAspectRatio(float(m_surface->width())/float(m_surface->height()));
}

void Qt3DGeometryTab::resetCamera()
{
    // TODO set this based on geometry bounding box
    m_camera->lens()->setPerspectiveProjection(45.0f, float(m_surface->width())/float(m_surface->height()), 0.1f, 1000.0f);
    m_camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
    m_camera->setPosition(QVector3D(0, 0, 4.0f));
}
