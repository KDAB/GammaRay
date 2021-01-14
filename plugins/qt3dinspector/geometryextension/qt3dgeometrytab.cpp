/*
  qt3dgeometrytab.cpp

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

#include "qt3dgeometrytab.h"
#include "ui_qt3dgeometrytab.h"
#include "qt3dgeometryextensioninterface.h"
#include "cameracontroller.h"
#include "attribute.h"
#include "buffermodel.h"

#include <ui/propertywidget.h>
#include <common/objectbroker.h>

#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QForwardRenderer>

#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBlendEquation>
#include <Qt3DRender/QBlendEquationArguments>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QCullFace>
#include <Qt3DRender/QDepthTest>
#include <Qt3DRender/QEffect>
#include <Qt3DRender/QFilterKey>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QGraphicsApiFilter>
#include <Qt3DRender/QMaterial>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickTriangleEvent>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QParameter>
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
#include <Qt3DCore/QTransform>

#include <QDebug>
#include <QOpenGLContext>
#include <QUrl>
#include <QToolBar>
#include <QWindow>

using namespace GammaRay;

// ### keep in sync with wireframe.vert/wireframe.frag
enum ShadingMode {
    ShadingModeFlat = 0,
    ShadingModePhong = 1,
    ShadingModeTexture = 2,
    ShadingModeNormal = 3,
    ShadingModeTangent = 4,
    ShadingModeColor = 5,
    ShadingModeWireframe = 6
};

Qt3DGeometryTab::Qt3DGeometryTab(PropertyWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Qt3DGeometryTab)
    , m_surface(nullptr)
    , m_aspectEngine(nullptr)
    , m_camera(nullptr)
    , m_geometryRenderer(nullptr)
    , m_es2lineRenderer(nullptr)
    , m_geometryTransform(nullptr)
    , m_cullMode(nullptr)
    , m_depthTest(nullptr)
    , m_normalsRenderPass(nullptr)
    , m_normalLength(nullptr)
    , m_shadingMode(nullptr)
    , m_bufferModel(new BufferModel(this))
{
    ui->setupUi(this);
    auto toolbar = new QToolBar(this);
    ui->topLayout->insertWidget(0, toolbar);

    toolbar->addAction(ui->actionViewGeometry);
    toolbar->addAction(ui->actionViewBuffers);
    toolbar->addSeparator();
    toolbar->addAction(ui->actionResetCam);
    toolbar->addSeparator();
    toolbar->addAction(ui->actionShowNormals);
    toolbar->addAction(ui->actionShowTangents);
    toolbar->addAction(ui->actionCullBack);
    toolbar->addSeparator();
    auto shadingModeLabel = toolbar->addWidget(new QLabel(tr("Shading:"), toolbar));
    m_shadingModeCombo = new QComboBox(toolbar);
    m_shadingModeCombo->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    auto shadingModeAction = toolbar->addWidget(m_shadingModeCombo);

    connect(ui->actionResetCam, &QAction::triggered, this, &Qt3DGeometryTab::resetCamera);

    connect(ui->actionShowNormals, &QAction::toggled, this, [this]() {
        if (m_normalsRenderPass) {
            m_normalsRenderPass->setEnabled(ui->actionShowNormals->isChecked());
        }
    });
    connect(ui->actionCullBack, &QAction::toggled, this, [this]() {
        if (m_cullMode) {
            m_cullMode->setMode(ui->actionCullBack->isChecked() ? Qt3DRender::QCullFace::Back :
                                Qt3DRender::QCullFace::NoCulling);
        }
    });
    connect(m_shadingModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() {
        const auto shadingMode = m_shadingModeCombo->currentData();
        if (!shadingMode.isValid() || !m_shadingMode || !m_cullMode)
            return;
        m_shadingMode->setValue(shadingMode);
        if (shadingMode.toInt() == ShadingModeWireframe) {
            ui->actionCullBack->setEnabled(false);
            m_cullMode->setMode(Qt3DRender::QCullFace::NoCulling);
            m_depthTest->setDepthFunction(Qt3DRender::QDepthTest::Always);
        } else {
            ui->actionCullBack->setEnabled(true);
            m_cullMode->setMode(ui->actionCullBack->isChecked() ? Qt3DRender::QCullFace::Back :
                                Qt3DRender::QCullFace::NoCulling);
            m_depthTest->setDepthFunction(Qt3DRender::QDepthTest::Less);
        }
    });

    auto viewGroup = new QActionGroup(this);
    viewGroup->setExclusive(true);
    viewGroup->addAction(ui->actionViewGeometry);
    viewGroup->addAction(ui->actionViewBuffers);
    connect(viewGroup, &QActionGroup::triggered, this, [this, shadingModeLabel, shadingModeAction]() {
        const auto geoView = ui->actionViewGeometry->isChecked();
        ui->stackedWidget->setCurrentWidget(geoView ? ui->geometryPage : ui->bufferPage);
        ui->actionResetCam->setVisible(geoView);
        ui->actionShowNormals->setVisible(geoView);
        ui->actionShowTangents->setVisible(geoView);
        ui->actionCullBack->setVisible(geoView);
        shadingModeLabel->setVisible(geoView);
        shadingModeAction->setVisible(geoView);
    });

    ui->bufferView->setModel(m_bufferModel);
    ui->bufferView->horizontalHeader()->setObjectName(QStringLiteral("bufferViewHeader"));
    connect(ui->bufferBox, QOverload<int>::of(&QComboBox::currentIndexChanged), m_bufferModel, &BufferModel::setBufferIndex);

    m_surface = new QWindow;
    m_surface->setFlags(Qt::Window | Qt::FramelessWindowHint);
    m_surface->setSurfaceType(QSurface::OpenGLSurface);
    const auto format = probeFormat();
    m_surface->setFormat(format);
    QSurfaceFormat::setDefaultFormat(format);
    m_surface->create();
    auto container = QWidget::createWindowContainer(m_surface, this);
    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    ui->geometryPage->layout()->addWidget(container);
    m_surface->installEventFilter(this);

    m_interface = ObjectBroker::object<Qt3DGeometryExtensionInterface *>(
        parent->objectBaseName() + ".qt3dGeometry");
    connect(m_interface, &Qt3DGeometryExtensionInterface::geometryDataChanged, this,
            &Qt3DGeometryTab::updateGeometry);
}

Qt3DGeometryTab::~Qt3DGeometryTab() = default;

bool Qt3DGeometryTab::eventFilter(QObject *receiver, QEvent *event)
{
    if (receiver != m_surface || event->type() != QEvent::Expose || m_aspectEngine)
        return QWidget::eventFilter(receiver, event);

    m_aspectEngine = new Qt3DCore::QAspectEngine(this);
    m_aspectEngine->registerAspect(new Qt3DRender::QRenderAspect);

    auto rootEntity = new Qt3DCore::QEntity;

    m_camera = new Qt3DRender::QCamera;
    resetCamera();

    // rendering
    auto forwardRenderer = new Qt3DExtras::QForwardRenderer;
    forwardRenderer->setClearColor(Qt::black);
    forwardRenderer->setCamera(m_camera);
    forwardRenderer->setSurface(m_surface);

    auto renderSettings = new Qt3DRender::QRenderSettings;
    renderSettings->setActiveFrameGraph(forwardRenderer);
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    renderSettings->pickingSettings()->setFaceOrientationPickingMode(Qt3DRender::QPickingSettings::FrontFace);
    renderSettings->pickingSettings()->setPickMethod(Qt3DRender::QPickingSettings::TrianglePicking);
    renderSettings->pickingSettings()->setPickResultMode(Qt3DRender::QPickingSettings::NearestPick);
#endif
    rootEntity->addComponent(renderSettings);

    auto skyboxEntity = new Qt3DCore::QEntity(rootEntity);
    auto skyBoxGeometry = new Qt3DExtras::QCuboidMesh;
    skyBoxGeometry->setXYMeshResolution(QSize(2, 2));
    skyBoxGeometry->setXZMeshResolution(QSize(2, 2));
    skyBoxGeometry->setYZMeshResolution(QSize(2, 2));
    auto skyboxTransform = new Qt3DCore::QTransform;
    skyboxTransform->setTranslation(m_camera->position());
    connect(m_camera, &Qt3DRender::QCamera::positionChanged, skyboxTransform,
            &Qt3DCore::QTransform::setTranslation);
    skyboxEntity->addComponent(skyBoxGeometry);
    skyboxEntity->addComponent(createSkyboxMaterial(rootEntity));
    skyboxEntity->addComponent(skyboxTransform);

    auto geometryEntity = new Qt3DCore::QEntity(rootEntity);
    m_geometryRenderer = new Qt3DRender::QGeometryRenderer;
    geometryEntity->addComponent(m_geometryRenderer);
    geometryEntity->addComponent(createMaterial(rootEntity));
    m_geometryTransform = new Qt3DCore::QTransform;
    geometryEntity->addComponent(m_geometryTransform);
    auto picker = new Qt3DRender::QObjectPicker;
    connect(picker, &Qt3DRender::QObjectPicker::clicked, this, &Qt3DGeometryTab::trianglePicked);
    geometryEntity->addComponent(picker);

    // fallback wireframe rendering with ES2
    if (m_usingES2Fallback) {
        auto es2lineEntity = new Qt3DCore::QEntity(rootEntity);
        m_es2lineRenderer = new Qt3DRender::QGeometryRenderer;
        es2lineEntity->addComponent(m_es2lineRenderer);
        es2lineEntity->addComponent(createES2WireframeMaterial(rootEntity));
        es2lineEntity->addComponent(m_geometryTransform);

        auto label = new QLabel(tr("<i>Using OpenGL ES2 fallback, wireframe rendering will be inaccurate.</i>"));
        label->setAlignment(Qt::AlignRight);
        label->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);
        ui->geometryPage->layout()->addWidget(label);

        ui->actionShowNormals->setToolTip(tr("Visualizing normals not available when running in OpenGL ES2 fallback mode."));
    }

    updateGeometry();

    auto lightEntity = new Qt3DCore::QEntity(rootEntity);
    auto light = new Qt3DRender::QPointLight(lightEntity);
    lightEntity->addComponent(light);
    auto lightTransform = new Qt3DCore::QTransform(lightEntity);
    lightTransform->setTranslation(m_camera->position());
    connect(m_camera, &Qt3DRender::QCamera::positionChanged, lightTransform, &Qt3DCore::QTransform::setTranslation);
    lightEntity->addComponent(lightTransform);

    // input handling
    m_aspectEngine->registerAspect(new Qt3DLogic::QLogicAspect);
    m_aspectEngine->registerAspect(new Qt3DInput::QInputAspect);
    auto inputSettings = new Qt3DInput::QInputSettings;
    inputSettings->setEventSource(m_surface);
    rootEntity->addComponent(inputSettings);

    auto camController = new CameraController(rootEntity);
    camController->setCamera(m_camera);

    m_aspectEngine->setRootEntity(Qt3DCore::QEntityPtr(rootEntity));
    return QWidget::eventFilter(receiver, event);
}

Qt3DCore::QComponent *Qt3DGeometryTab::createMaterial(Qt3DCore::QNode *parent)
{
    auto material = new Qt3DRender::QMaterial(parent);

    // wireframe render pass
    m_cullMode = new Qt3DRender::QCullFace;
    m_cullMode->setMode(ui->actionCullBack->isChecked() ? Qt3DRender::QCullFace::Back : Qt3DRender::QCullFace::NoCulling);

    m_shadingMode = new Qt3DRender::QParameter(QStringLiteral("shadingMode"), m_shadingModeCombo->currentData(), material);
    material->addParameter(m_shadingMode);

    auto gl3WireframeShader = new Qt3DRender::QShaderProgram;
    gl3WireframeShader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/gl3/wireframe.vert"))));
    gl3WireframeShader->setGeometryShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/gl3/wireframe.geom"))));
    gl3WireframeShader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/gl3/wireframe.frag"))));
    auto es2WireframeShader = new Qt3DRender::QShaderProgram;
    es2WireframeShader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/es2/surface.vert"))));
    es2WireframeShader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/es2/surface.frag"))));

    auto blendEquationArgs = new Qt3DRender::QBlendEquationArguments;
    blendEquationArgs->setSourceRgb(Qt3DRender::QBlendEquationArguments::SourceAlpha);
    blendEquationArgs->setDestinationRgb(Qt3DRender::QBlendEquationArguments::OneMinusSourceAlpha);
    auto blendEquation = new Qt3DRender::QBlendEquation;
    blendEquation->setBlendFunction(Qt3DRender::QBlendEquation::Add);
    m_depthTest = new Qt3DRender::QDepthTest;
    m_depthTest->setDepthFunction(Qt3DRender::QDepthTest::Less);

    auto gl3WireframeRenderPass = new Qt3DRender::QRenderPass;
    gl3WireframeRenderPass->setShaderProgram(gl3WireframeShader);
    gl3WireframeRenderPass->addRenderState(m_cullMode);
    gl3WireframeRenderPass->addRenderState(blendEquationArgs);
    gl3WireframeRenderPass->addRenderState(blendEquation);
    gl3WireframeRenderPass->addRenderState(m_depthTest);
    auto es2WireframeRenderPass = new Qt3DRender::QRenderPass;
    es2WireframeRenderPass->setShaderProgram(es2WireframeShader);
    es2WireframeRenderPass->addRenderState(m_cullMode);
    es2WireframeRenderPass->addRenderState(blendEquationArgs);
    es2WireframeRenderPass->addRenderState(blendEquation);
    es2WireframeRenderPass->addRenderState(m_depthTest);

    // normal render pass
    m_normalLength = new Qt3DRender::QParameter(QStringLiteral("normalLength"), 0.1, material);
    material->addParameter(m_normalLength);

    auto normalsShader = new Qt3DRender::QShaderProgram;
    normalsShader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral(
                                                                                       "qrc:/gammaray/qt3dinspector/geometryextension/gl3/passthrough.vert"))));
    normalsShader->setGeometryShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral(
                                                                                         "qrc:/gammaray/qt3dinspector/geometryextension/gl3/normals.geom"))));
    normalsShader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral(
                                                                                         "qrc:/gammaray/qt3dinspector/geometryextension/gl3/normals.frag"))));

    m_normalsRenderPass = new Qt3DRender::QRenderPass;
    m_normalsRenderPass->setShaderProgram(normalsShader);
    m_normalsRenderPass->setEnabled(ui->actionShowNormals->isChecked());

    auto filterKey = new Qt3DRender::QFilterKey(material);
    filterKey->setName(QStringLiteral("renderingStyle"));
    filterKey->setValue(QStringLiteral("forward"));

    auto gl3Technique = new Qt3DRender::QTechnique;
    gl3Technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    gl3Technique->graphicsApiFilter()->setMajorVersion(3);
    gl3Technique->graphicsApiFilter()->setMinorVersion(3);
    gl3Technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);
    gl3Technique->addRenderPass(gl3WireframeRenderPass);
    gl3Technique->addRenderPass(m_normalsRenderPass);
    gl3Technique->addFilterKey(filterKey);
    auto es2Technique = new Qt3DRender::QTechnique;
    es2Technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGLES);
    es2Technique->graphicsApiFilter()->setMajorVersion(2);
    es2Technique->graphicsApiFilter()->setMinorVersion(0);
    es2Technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::NoProfile);
    es2Technique->addRenderPass(es2WireframeRenderPass);
    es2Technique->addFilterKey(filterKey);

    auto effect = new Qt3DRender::QEffect;
    effect->addTechnique(gl3Technique);
    effect->addTechnique(es2Technique);

    material->setEffect(effect);
    return material;
}

Qt3DCore::QComponent* Qt3DGeometryTab::createES2WireframeMaterial(Qt3DCore::QNode *parent)
{
    auto material = new Qt3DRender::QMaterial(parent);

    auto shader = new Qt3DRender::QShaderProgram;
    shader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/es2/wireframe.vert"))));
    shader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/es2/wireframe.frag"))));

    auto renderPass = new Qt3DRender::QRenderPass;
    renderPass->setShaderProgram(shader);
    renderPass->addRenderState(m_cullMode);
    renderPass->addRenderState(m_depthTest);

    auto filterKey = new Qt3DRender::QFilterKey(material);
    filterKey->setName(QStringLiteral("renderingStyle"));
    filterKey->setValue(QStringLiteral("forward"));

    auto technique = new Qt3DRender::QTechnique;
    technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGLES);
    technique->graphicsApiFilter()->setMajorVersion(2);
    technique->graphicsApiFilter()->setMinorVersion(0);
    technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::NoProfile);
    technique->addRenderPass(renderPass);
    technique->addFilterKey(filterKey);

    auto effect = new Qt3DRender::QEffect;
    effect->addTechnique(technique);

    material->setEffect(effect);
    return material;
}

Qt3DCore::QComponent *Qt3DGeometryTab::createSkyboxMaterial(Qt3DCore::QNode *parent)
{
    auto material = new Qt3DRender::QMaterial(parent);

    auto cullFront = new Qt3DRender::QCullFace;
    cullFront->setMode(Qt3DRender::QCullFace::Front);
    auto depthTest = new Qt3DRender::QDepthTest;
    depthTest->setDepthFunction(Qt3DRender::QDepthTest::LessOrEqual);

    auto gl3Shader = new Qt3DRender::QShaderProgram;
    gl3Shader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/gl3/skybox.vert"))));
    gl3Shader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/gl3/skybox.frag"))));
    auto es2Shader = new Qt3DRender::QShaderProgram;
    es2Shader->setVertexShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/es2/skybox.vert"))));
    es2Shader->setFragmentShaderCode(Qt3DRender::QShaderProgram::loadSource(QUrl(QStringLiteral("qrc:/gammaray/qt3dinspector/geometryextension/es2/skybox.frag"))));

    auto gl3RenderPass = new Qt3DRender::QRenderPass;
    gl3RenderPass->setShaderProgram(gl3Shader);
    gl3RenderPass->addRenderState(cullFront);
    gl3RenderPass->addRenderState(depthTest);
    auto es2RenderPass = new Qt3DRender::QRenderPass;
    es2RenderPass->setShaderProgram(es2Shader);
    es2RenderPass->addRenderState(cullFront);
    es2RenderPass->addRenderState(depthTest);

    auto filterKey = new Qt3DRender::QFilterKey(material);
    filterKey->setName(QStringLiteral("renderingStyle"));
    filterKey->setValue(QStringLiteral("forward"));

    auto gl3Technique = new Qt3DRender::QTechnique;
    gl3Technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGL);
    gl3Technique->graphicsApiFilter()->setMajorVersion(3);
    gl3Technique->graphicsApiFilter()->setMinorVersion(3);
    gl3Technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::CoreProfile);
    gl3Technique->addRenderPass(gl3RenderPass);
    gl3Technique->addFilterKey(filterKey);
    auto es2Technique = new Qt3DRender::QTechnique;
    es2Technique->graphicsApiFilter()->setApi(Qt3DRender::QGraphicsApiFilter::OpenGLES);
    es2Technique->graphicsApiFilter()->setMajorVersion(2);
    es2Technique->graphicsApiFilter()->setMinorVersion(0);
    es2Technique->graphicsApiFilter()->setProfile(Qt3DRender::QGraphicsApiFilter::NoProfile);
    es2Technique->addRenderPass(es2RenderPass);
    es2Technique->addFilterKey(filterKey);

    auto effect = new Qt3DRender::QEffect;
    effect->addTechnique(gl3Technique);
    effect->addTechnique(es2Technique);

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
    ui->actionShowNormals->setEnabled(false);
    ui->actionShowTangents->setEnabled(false);
    ui->bufferBox->clear();
    const auto prevShadingMode = m_shadingModeCombo->currentData();
    m_shadingModeCombo->clear();
    m_shadingModeCombo->addItem(tr("Flat"), ShadingModeFlat);
    m_shadingModeCombo->addItem(tr("Wireframe"), ShadingModeWireframe);

    if (!m_geometryRenderer)
        return;

    const auto geo = m_interface->geometryData();
    m_bufferModel->setGeometryData(geo);

    auto geometry = new Qt3DRender::QGeometry();
    QVector<Qt3DRender::QBuffer *> buffers;
    buffers.reserve(geo.buffers.size());
    for (const auto &bufferData : geo.buffers) {
        auto buffer = new Qt3DRender::QBuffer(bufferData.type, geometry);
        buffer->setData(bufferData.data);
        buffers.push_back(buffer);
        ui->bufferBox->addItem(bufferData.name, QVariant::fromValue(buffer));
    }

    for (const auto &attrData : geo.attributes) {
        if (attrData.name == Qt3DRender::QAttribute::defaultPositionAttributeName()) {
            auto posAttr = new Qt3DRender::QAttribute();
            posAttr->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
            posAttr->setBuffer(buffers.at(attrData.bufferIndex));
            setupAttribute(posAttr, attrData);
            posAttr->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());
            geometry->addAttribute(posAttr);
            geometry->setBoundingVolumePositionAttribute(posAttr);
            computeBoundingVolume(attrData, posAttr->buffer()->data());
            m_geometryTransform->setTranslation(-m_boundingVolume.center());
            m_normalLength->setValue(0.025 * m_boundingVolume.radius());
        } else if (attrData.name == Qt3DRender::QAttribute::defaultNormalAttributeName()) {
            auto normalAttr = new Qt3DRender::QAttribute();
            normalAttr->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
            normalAttr->setBuffer(buffers.at(attrData.bufferIndex));
            setupAttribute(normalAttr, attrData);
            normalAttr->setName(Qt3DRender::QAttribute::defaultNormalAttributeName());
            geometry->addAttribute(normalAttr);
            ui->actionShowNormals->setEnabled(!m_usingES2Fallback);
            m_shadingModeCombo->addItem(tr("Phong"), ShadingModePhong);
            m_shadingModeCombo->addItem(tr("Normal"), ShadingModeNormal);
        } else if (attrData.attributeType == Qt3DRender::QAttribute::IndexAttribute) {
            auto indexAttr = new Qt3DRender::QAttribute();
            indexAttr->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
            indexAttr->setBuffer(buffers.at(attrData.bufferIndex));
            setupAttribute(indexAttr, attrData);
            geometry->addAttribute(indexAttr);
        } else if (attrData.name == Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName()) {
            auto texCoordAttr = new Qt3DRender::QAttribute();
            texCoordAttr->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
            texCoordAttr->setBuffer(buffers.at(attrData.bufferIndex));
            setupAttribute(texCoordAttr, attrData);
            texCoordAttr->setName(Qt3DRender::QAttribute::defaultTextureCoordinateAttributeName());
            geometry->addAttribute(texCoordAttr);
            m_shadingModeCombo->addItem(tr("Texture Coordinate"), ShadingModeTexture);
        } else if (attrData.name == Qt3DRender::QAttribute::defaultTangentAttributeName()) {
            auto tangentAttr = new Qt3DRender::QAttribute();
            tangentAttr->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
            tangentAttr->setBuffer(buffers.at(attrData.bufferIndex));
            setupAttribute(tangentAttr, attrData);
            tangentAttr->setName(Qt3DRender::QAttribute::defaultTangentAttributeName());
            geometry->addAttribute(tangentAttr);
            m_shadingModeCombo->addItem(tr("Tangent"), ShadingModeTangent);
        } else if (attrData.name == Qt3DRender::QAttribute::defaultColorAttributeName()) {
            auto colorAttr = new Qt3DRender::QAttribute();
            colorAttr->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
            colorAttr->setBuffer(buffers.at(attrData.bufferIndex));
            setupAttribute(colorAttr, attrData);
            colorAttr->setName(Qt3DRender::QAttribute::defaultColorAttributeName());
            geometry->addAttribute(colorAttr);
            m_shadingModeCombo->addItem(tr("Color"), ShadingModeColor);
        }
    }

    m_geometryRenderer->setInstanceCount(1);
    m_geometryRenderer->setIndexOffset(0);
    m_geometryRenderer->setFirstInstance(0);
    m_geometryRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);

    if (m_es2lineRenderer) {
        m_es2lineRenderer->setInstanceCount(1);
        m_es2lineRenderer->setIndexOffset(0);
        m_es2lineRenderer->setFirstInstance(0);
        m_es2lineRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::LineLoop);
        m_es2lineRenderer->setGeometry(geometry);
    }

    auto oldGeometry = m_geometryRenderer->geometry();
    m_geometryRenderer->setGeometry(geometry);
    delete oldGeometry;

    const auto prevShadingModeIdx = m_shadingModeCombo->findData(prevShadingMode);
    if (prevShadingModeIdx >= 0)
        m_shadingModeCombo->setCurrentIndex(prevShadingModeIdx);

    resetCamera();
}

void Qt3DGeometryTab::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (m_surface && m_camera)
        m_camera->lens()->setAspectRatio(float(m_surface->width()) / float(m_surface->height()));
}

void Qt3DGeometryTab::resetCamera()
{
    m_camera->lens()->setPerspectiveProjection(45.0f,
                                               float(m_surface->width()) / float(m_surface->height()), 0.1f,
                                               1000.0f);
    m_camera->setViewCenter(QVector3D(0.0f, 0.0f, 0.0f));
    m_camera->setUpVector(QVector3D(0.0f, 1.0f, 0.0f));
    m_camera->setPosition(QVector3D(0, 0, m_boundingVolume.radius() * 2.5f));
}

void Qt3DGeometryTab::computeBoundingVolume(const Qt3DGeometryAttributeData &vertexAttr,
                                            const QByteArray &bufferData)
{
    m_boundingVolume = BoundingVolume();
    QVector3D v;
    const auto vertexSize = std::max(vertexAttr.vertexSize, 1u);
    const auto stride = std::max(vertexAttr.byteStride, (uint)Attribute::size(vertexAttr.vertexBaseType) * vertexSize);
    for (unsigned int i = 0; i < vertexAttr.count; ++i) {
        const char *c = bufferData.constData() + vertexAttr.byteOffset + i * stride;
        switch (vertexAttr.vertexBaseType) {
        case Qt3DRender::QAttribute::Float:
        {
            // cppcheck-suppress invalidPointerCast
            auto f = reinterpret_cast<const float *>(c);
            v.setX(*f);
            ++f;
            v.setY(*f);
            ++f;
            v.setZ(*f);
            break;
        }
        default:
            qWarning() << "Vertex type" << vertexAttr.vertexBaseType << "not implemented yet";
            return;
        }
        m_boundingVolume.addPoint(v);
    }
}

bool Qt3DGeometryTab::isIndexBuffer(unsigned int bufferIndex) const
{
    foreach (const auto &attr, m_interface->geometryData().attributes) {
        if (attr.bufferIndex == bufferIndex)
            return attr.attributeType == Qt3DRender::QAttribute::IndexAttribute;
    }
    return false;
}


void Qt3DGeometryTab::trianglePicked(Qt3DRender::QPickEvent* pick)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    if (pick->button() != Qt3DRender::QPickEvent::LeftButton)
        return;
    const auto trianglePick = qobject_cast<Qt3DRender::QPickTriangleEvent*>(pick);

    qDebug() << trianglePick << trianglePick->vertex1Index() << trianglePick->vertex2Index() << trianglePick->vertex3Index() << trianglePick->localIntersection() << trianglePick->triangleIndex() << m_interface->geometryData().buffers.at(ui->bufferBox->currentIndex()).type << ui->bufferBox->currentIndex();
    auto selModel = ui->bufferView->selectionModel();
    selModel->clear();
    if (isIndexBuffer(ui->bufferBox->currentIndex())) {
        selModel->select(selModel->model()->index(trianglePick->triangleIndex() * 3, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
        selModel->select(selModel->model()->index(trianglePick->triangleIndex() * 3 + 1, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
        selModel->select(selModel->model()->index(trianglePick->triangleIndex() * 3 + 2, 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    } else {
        // TODO we could pick one here based on pick->localIntersection() and smallest distance to one of the three candidates?
        selModel->select(selModel->model()->index(trianglePick->vertex1Index(), 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
        selModel->select(selModel->model()->index(trianglePick->vertex2Index(), 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
        selModel->select(selModel->model()->index(trianglePick->vertex3Index(), 0), QItemSelectionModel::Rows | QItemSelectionModel::Select);
    }

    foreach (const auto &row, selModel->selectedRows())
        ui->bufferView->scrollTo(row, QAbstractItemView::EnsureVisible);
#else
    Q_UNUSED(pick);
#endif
}

QSurfaceFormat Qt3DGeometryTab::probeFormat() const
{
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    format.setSamples(4); // ???
    format.setStencilBufferSize(8); // ???

    // try GL3 first
    format.setRenderableType(QSurfaceFormat::OpenGL);
    format.setMajorVersion(3);
    format.setMinorVersion(0);
    format.setProfile(QSurfaceFormat::CoreProfile);

    QOpenGLContext context;
    context.setScreen(window()->windowHandle()->screen());
    context.setFormat(format);
    if (context.create()) {
        qDebug() << "Tried GL3, got:" << context.format() << context.format().renderableType();
        m_usingES2Fallback = context.format().renderableType() == QSurfaceFormat::OpenGLES;
        return format;
    }

    // fall back to ES2
    m_usingES2Fallback = true;
    format.setRenderableType(QSurfaceFormat::OpenGLES);
    format.setMajorVersion(2);
    format.setMinorVersion(0);
    format.setProfile(QSurfaceFormat::NoProfile);
    return format;
}
