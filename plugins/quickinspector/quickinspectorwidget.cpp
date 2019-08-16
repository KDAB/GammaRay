/*
  quickinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "quickinspectorwidget.h"
#include "quickinspectorclient.h"
#include "quickclientitemmodel.h"
#include "quickitemtreewatcher.h"
#include "quickitemmodelroles.h"
#include "quickscenepreviewwidget.h"
#ifndef QT_NO_OPENGL
#include "geometryextension/sggeometrytab.h"
#include "materialextension/materialextensionclient.h"
#include "materialextension/materialtab.h"
#include "textureextension/texturetab.h"
#endif
#include "quickitemdelegate.h"
#include "ui_quickinspectorwidget.h"

#include <common/objectbroker.h>
#include <common/objectmodel.h>
#include <common/objectid.h>
#include <common/remotemodelroles.h>
#include <common/sourcelocation.h>

#include <ui/clientdecorationidentityproxymodel.h>
#include <ui/contextmenuextension.h>
#include <ui/paintbufferviewer.h>
#include <ui/searchlinecontroller.h>

#include <QEvent>
#include <QLabel>
#include <QMenu>
#include <qmath.h>
#include <QRectF>
#include <QtCore/qglobal.h>
#include <QPropertyAnimation>
#include <QSettings>
#include <QFileDialog>
#include <QDebug>

using namespace GammaRay;

static QObject *createQuickInspectorClient(const QString & /*name*/, QObject *parent)
{
    return new QuickInspectorClient(parent);
}

#ifndef QT_NO_OPENGL
static QObject *createMaterialExtension(const QString &name, QObject *parent)
{
    return new MaterialExtensionClient(name, parent);
}
#endif

static QAction *createSeparator(QObject *parent)
{
    QAction *action = new QAction(parent);
    action->setSeparator(true);
    return action;
}

QuickInspectorWidget::QuickInspectorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::QuickInspectorWidget)
    , m_state(QuickInspectorWidget::WaitingAll)
    , m_stateManager(this)
{
    qRegisterMetaType<QuickInspectorWidget::StateFlag>();

    ui->setupUi(this);

    ObjectBroker::registerClientObjectFactoryCallback<QuickInspectorInterface *>(
        createQuickInspectorClient);

    m_interface = ObjectBroker::object<QuickInspectorInterface *>();

    ui->windowComboBox->setModel(ObjectBroker::model(QStringLiteral(
                                                         "com.kdab.GammaRay.QuickWindowModel")));
    connect(ui->windowComboBox, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            m_interface, &QuickInspectorInterface::selectWindow);
    if (ui->windowComboBox->currentIndex() >= 0)
        m_interface->selectWindow(ui->windowComboBox->currentIndex());

    auto model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickItemModel"));
    auto proxy = new QuickClientItemModel(this);
    proxy->setSourceModel(model);
    ui->itemTreeView->header()->setObjectName("quickItemTreeViewHeader");
    ui->itemTreeView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->itemTreeView->setModel(proxy);
    ui->itemTreeView->setItemDelegate(new QuickItemDelegate(ui->itemTreeView));
    new SearchLineController(ui->itemTreeSearchLine, proxy);
    QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(proxy);
    ui->itemTreeView->setSelectionModel(selectionModel);
    connect(selectionModel, &QItemSelectionModel::selectionChanged,
            this, &QuickInspectorWidget::itemSelectionChanged);
    connect(proxy, &QAbstractItemModel::dataChanged,
            this, &QuickInspectorWidget::itemModelDataChanged);

    model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickSceneGraphModel"));
    auto *clientSceneGraphModel = new ClientDecorationIdentityProxyModel(this);
    clientSceneGraphModel->setSourceModel(model);
    ui->sgTreeView->header()->setObjectName("sceneGraphTreeViewHeader");
    ui->sgTreeView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->sgTreeView->setModel(clientSceneGraphModel);
    new SearchLineController(ui->sgTreeSearchLine, clientSceneGraphModel);
    QItemSelectionModel *sgSelectionModel = ObjectBroker::selectionModel(clientSceneGraphModel);
    ui->sgTreeView->setSelectionModel(sgSelectionModel);
    connect(sgSelectionModel, &QItemSelectionModel::selectionChanged,
            this, &QuickInspectorWidget::sgSelectionChanged);

    new QuickItemTreeWatcher(ui->itemTreeView, ui->sgTreeView, this);

    m_scenePreviewWidget = new QuickSceneControlWidget(m_interface, this);
    m_scenePreviewWidget->previewWidget()->setPickSourceModel(proxy);
    m_scenePreviewWidget->previewWidget()->setFlagRole(QuickItemModelRole::ItemFlags);
    m_scenePreviewWidget->previewWidget()->setInvisibleMask(QuickItemModelRole::Invisible | QuickItemModelRole::ZeroSize);

    ui->itemPropertyWidget->setObjectBaseName(QStringLiteral("com.kdab.GammaRay.QuickItem"));
    ui->sgPropertyWidget->setObjectBaseName(QStringLiteral("com.kdab.GammaRay.QuickSceneGraph"));

    ui->previewTreeSplitter->addWidget(m_scenePreviewWidget);

    connect(m_interface, &QuickInspectorInterface::features, this, &QuickInspectorWidget::setFeatures);
    connect(m_interface, &QuickInspectorInterface::serverSideDecorationChanged, m_scenePreviewWidget, &QuickSceneControlWidget::setServerSideDecorationsEnabled);
    connect(m_interface, &QuickInspectorInterface::overlaySettings, this, &QuickInspectorWidget::setOverlaySettings);

    connect(m_interface, &QuickInspectorInterface::slowModeChanged, this, &QuickInspectorWidget::setSlowMode);

    connect(ui->itemTreeView, &QWidget::customContextMenuRequested,
            this, &QuickInspectorWidget::itemContextMenu);

    m_interface->checkFeatures();
    m_interface->checkOverlaySettings();
    m_interface->checkSlowMode();

    addActions(m_scenePreviewWidget->actions());
    addAction(createSeparator(this));
    addAction(ui->actionAnalyzePainting);
    addAction(createSeparator(this));
    addAction(ui->actionSaveAsImage);
    addAction(ui->actionSaveAsImageWithDecoration);
    addAction(createSeparator(this));
    addAction(ui->actionSlowDownMode);

    m_stateManager.setDefaultSizes(ui->mainSplitter, UISizeVector() << "50%" << "50%");
    m_stateManager.setDefaultSizes(ui->previewTreeSplitter, UISizeVector() << "50%" << "50%");

    connect(ui->actionSaveAsImage, &QAction::triggered, this, &QuickInspectorWidget::saveAsImage);
    connect(ui->actionSaveAsImageWithDecoration, &QAction::triggered, this, &QuickInspectorWidget::saveAsImage);
    connect(ui->actionAnalyzePainting, &QAction::triggered, this, [this]() {
        m_interface->analyzePainting();
        auto viewer = new PaintBufferViewer(QStringLiteral("com.kdab.GammaRay.QuickPaintAnalyzer"), this);
        viewer->show();
    });
    connect(ui->actionSlowDownMode, &QAction::triggered, m_interface, &QuickInspectorInterface::setSlowMode);
    connect(ui->itemPropertyWidget, &PropertyWidget::tabsUpdated, this, &QuickInspectorWidget::resetState);
    connect(ui->sgPropertyWidget, &PropertyWidget::tabsUpdated, this, &QuickInspectorWidget::resetState);
    connect(m_scenePreviewWidget, &QuickSceneControlWidget::stateChanged, this, &QuickInspectorWidget::saveState);
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, &QuickInspectorWidget::saveState);
    connect(m_scenePreviewWidget->previewWidget(), &RemoteViewWidget::frameChanged, this, &QuickInspectorWidget::updateActions);

    updateActions();
}

QuickInspectorWidget::~QuickInspectorWidget() = default;

void QuickInspectorWidget::saveTargetState(QSettings *settings) const
{
    if (m_state != QuickInspectorWidget::Ready)
        return;

    settings->setValue("tabIndex", ui->tabWidget->currentIndex());
    settings->setValue("remoteViewState", m_scenePreviewWidget->previewWidget()->saveState());
}

void QuickInspectorWidget::restoreTargetState(QSettings *settings)
{
    if (m_state != QuickInspectorWidget::Ready)
        return;

    ui->tabWidget->setCurrentIndex(settings->value("tabIndex", 0).toInt());

    // Delay those changes as they can lead to recursive save/load issue (ie, speak to server)
    QMetaObject::invokeMethod(m_scenePreviewWidget->previewWidget(), "restoreState", Qt::QueuedConnection,
                              Q_ARG(QByteArray, settings->value("remoteViewState").toByteArray()));
}

void QuickInspectorWidget::setFeatures(QuickInspectorInterface::Features features)
{
    m_scenePreviewWidget->setSupportsCustomRenderModes(features);
    ui->actionAnalyzePainting->setEnabled(features & QuickInspectorInterface::AnalyzePainting);
    stateReceived(QuickInspectorWidget::WaitingFeatures);
}

void QuickInspectorWidget::setOverlaySettings(const GammaRay::QuickDecorationsSettings &settings)
{
    m_scenePreviewWidget->setOverlaySettingsState(settings);
    stateReceived(QuickInspectorWidget::WaitingOverlaySettings);
}

void QuickInspectorWidget::setSlowMode(bool slow)
{
    ui->actionSlowDownMode->setChecked(slow);
}

void QuickInspectorWidget::itemSelectionChanged(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;
    const QModelIndex &index = selection.first().topLeft();
    ui->itemTreeView->scrollTo(index);
}

void QuickInspectorWidget::sgSelectionChanged(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;
    const auto &index = selection.first().topLeft();
    ui->sgTreeView->scrollTo(index);
}

void QuickInspectorWidget::itemModelDataChanged(const QModelIndex &topLeft,
                                                const QModelIndex &bottomRight,
                                                const QVector<int> &roles)
{
    if (!roles.contains(QuickItemModelRole::ItemEvent))
        return;

    for (int i = topLeft.row(); i <= bottomRight.row(); i++) {
        const QModelIndex index = ui->itemTreeView->model()->index(i, 0, topLeft.parent());
        const auto state = index.data(RemoteModelRole::LoadingState).value<RemoteModelNodeState::NodeStates>();
        if (state & RemoteModelNodeState::Empty || ~state & RemoteModelNodeState::Outdated)
            continue;

        auto *colorAnimation = new QVariantAnimation(this);
        QPersistentModelIndex persistentIndex(index);
        connect(colorAnimation, &QVariantAnimation::valueChanged,
                ui->itemTreeView->itemDelegate(), [persistentIndex, this](const QVariant &value) {
            qobject_cast<QuickItemDelegate *>(ui->itemTreeView->itemDelegate())->setTextColor(value,
                                                                                              persistentIndex);
        });
        colorAnimation->setStartValue(QColor(129, 0, 129));
        colorAnimation->setEndValue(QColor(129, 0, 129, 0));
        colorAnimation->setDuration(2000);
        colorAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
}

void QuickInspectorUiFactory::initUi()
{
#ifndef QT_NO_OPENGL
    ObjectBroker::registerClientObjectFactoryCallback<MaterialExtensionInterface *>(
        createMaterialExtension);

    PropertyWidget::registerTab<MaterialTab>(QStringLiteral("material"), tr("Material"));

    PropertyWidget::registerTab<SGGeometryTab>(QStringLiteral("sgGeometry"), tr("Geometry"));
    PropertyWidget::registerTab<TextureTab>(QStringLiteral("texture"), tr("Texture"));
#endif
}

void GammaRay::QuickInspectorWidget::itemContextMenu(const QPoint &pos)
{
    const QModelIndex index = ui->itemTreeView->indexAt(pos);
    if (!index.isValid())
        return;

    QMenu contextMenu;

    const auto objectId = index.data(ObjectModel::ObjectIdRole).value<ObjectId>();
    ContextMenuExtension ext(objectId);
    ext.setLocation(ContextMenuExtension::Creation, index.data(
                        ObjectModel::CreationLocationRole).value<SourceLocation>());
    ext.setLocation(ContextMenuExtension::Declaration,
                    index.data(ObjectModel::DeclarationLocationRole).value<SourceLocation>());
    ext.populateMenu(&contextMenu);
    contextMenu.exec(ui->itemTreeView->viewport()->mapToGlobal(pos));
}

void QuickInspectorWidget::stateReceived(GammaRay::QuickInspectorWidget::StateFlag flag)
{
    if (!m_state.testFlag(flag)) {
        return;
    }

    m_state &= ~flag;

    if (m_state == QuickInspectorWidget::WaitingApply) {
        QMetaObject::invokeMethod(this, "stateReceived", Qt::QueuedConnection, Q_ARG(GammaRay::QuickInspectorWidget::StateFlag, GammaRay::QuickInspectorWidget::WaitingApply));
    } else if (m_state == QuickInspectorWidget::Ready) {
        resetState();
    }
}

void QuickInspectorWidget::resetState()
{
    if (m_state != QuickInspectorWidget::Ready)
        return;

    m_stateManager.reset();
}

void QuickInspectorWidget::saveState()
{
    if (m_state != QuickInspectorWidget::Ready)
        return;

    m_stateManager.saveState();
}

void QuickInspectorWidget::saveAsImage()
{
    const QString fileName
        = QFileDialog::getSaveFileName(
        this,
        tr("Save As Image"),
        QString(),
        tr("Image Files (*.png *.jpg)"));

    if (fileName.isEmpty())
        return;

    const CompleteFrameRequest request(fileName,
                                       sender() ==  ui->actionSaveAsImageWithDecoration);
    m_scenePreviewWidget->previewWidget()->requestCompleteFrame(request);
}

void QuickInspectorWidget::updateActions()
{
    ui->actionSaveAsImage->setEnabled(m_scenePreviewWidget->previewWidget()->hasValidFrame());
    ui->actionSaveAsImageWithDecoration->setEnabled(ui->actionSaveAsImage->isEnabled());
}
