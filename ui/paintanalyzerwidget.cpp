/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "paintanalyzerwidget.h"
#include "ui_paintanalyzerwidget.h"
#include "paintbufferclientmodel.h"

#include <ui/clientpropertymodel.h>
#include <ui/contextmenuextension.h>
#include <ui/searchlinecontroller.h>
#include <ui/propertyeditor/propertyeditordelegate.h>
#include <ui/uiresources.h>

#include <common/paintanalyzerinterface.h>
#include <common/paintbuffermodelroles.h>
#include <common/objectbroker.h>
#include <common/sourcelocation.h>

#include <QComboBox>
#include <QDebug>
#include <QLabel>
#include <QMenu>
#include <QToolBar>

using namespace GammaRay;

PaintAnalyzerWidget::PaintAnalyzerWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PaintAnalyzerWidget)
    , m_iface(nullptr)
{
    ui->setupUi(this);
    ui->commandView->header()->setObjectName("commandViewHeader");
    ui->commandView->setItemDelegate(new PropertyEditorDelegate(this));
    ui->commandView->setStretchLastSection(false);
    ui->commandView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->commandView->setDeferredResizeMode(1, QHeaderView::Stretch);
    ui->commandView->setDeferredResizeMode(2, QHeaderView::ResizeToContents);

    ui->argumentView->setItemDelegate(new PropertyEditorDelegate(this));
    ui->stackTraceView->setItemDelegate(new PropertyEditorDelegate(this));

    auto toolbar = new QToolBar;
    // Our icons are 16x16 and support hidpi, so let force iconSize on every styles
    toolbar->setIconSize(QSize(16, 16));
    toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui->replayContainer->setMenuBar(toolbar);

    foreach (auto action, ui->replayWidget->interactionModeActions()->actions())
        toolbar->addAction(action);
    toolbar->addSeparator();

    toolbar->addAction(ui->replayWidget->zoomOutAction());
    auto zoom = new QComboBox;
    zoom->setModel(ui->replayWidget->zoomLevelModel());
    toolbar->addWidget(zoom);
    toolbar->addAction(ui->replayWidget->zoomInAction());
    toolbar->addSeparator();
    toolbar->addAction(ui->actionShowClipArea);

    ui->replayWidget->setSupportedInteractionModes(
        RemoteViewWidget::ViewInteraction | RemoteViewWidget::Measuring | RemoteViewWidget::ColorPicking);

    ui->paintAnalyzerSplitter->setStretchFactor(0, 1);
    ui->paintAnalyzerSplitter->setStretchFactor(1, 2);

    connect(zoom, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            ui->replayWidget, &RemoteViewWidget::setZoomLevel);
    connect(ui->replayWidget, &RemoteViewWidget::zoomLevelChanged, zoom, &QComboBox::setCurrentIndex);
    zoom->setCurrentIndex(ui->replayWidget->zoomLevelIndex());

    ui->actionShowClipArea->setIcon(UIResources::themedIcon(QLatin1String("visualize-clipping.png")));
    connect(ui->actionShowClipArea, &QAction::toggled, ui->replayWidget, &PaintAnalyzerReplayView::setShowClipArea);
    ui->actionShowClipArea->setChecked(ui->replayWidget->showClipArea());

    connect(ui->commandView, &QWidget::customContextMenuRequested, this, &PaintAnalyzerWidget::commandContextMenu);
    connect(ui->stackTraceView, &QWidget::customContextMenuRequested, this, &PaintAnalyzerWidget::stackTraceContextMenu);
}

PaintAnalyzerWidget::~PaintAnalyzerWidget() = default;

void PaintAnalyzerWidget::setBaseName(const QString &name)
{
    auto model = ObjectBroker::model(name + QStringLiteral(".paintBufferModel"));
    auto proxy = new PaintBufferClientModel(this);
    proxy->setSourceModel(model);
    ui->commandView->setModel(proxy);
    ui->commandView->setSelectionModel(ObjectBroker::selectionModel(proxy));
    new SearchLineController(ui->commandSearchLine, proxy);

    auto clientPropModel = new ClientPropertyModel(this);
    clientPropModel->setSourceModel(ObjectBroker::model(name + QStringLiteral(".argumentProperties")));
    ui->argumentView->setModel(clientPropModel);
    ui->stackTraceView->setModel(ObjectBroker::model(name + QStringLiteral(".stackTrace")));

    ui->replayWidget->setName(name + QStringLiteral(".remoteView"));

    m_iface = ObjectBroker::object<PaintAnalyzerInterface*>(name);
    connect(m_iface, &PaintAnalyzerInterface::hasArgumentDetailsChanged, this, &PaintAnalyzerWidget::detailsChanged);
    connect(m_iface, &PaintAnalyzerInterface::hasStackTraceChanged, this, &PaintAnalyzerWidget::detailsChanged);
    detailsChanged();
}

void PaintAnalyzerWidget::detailsChanged()
{
    const auto hasAnyDetails = m_iface->hasArgumentDetails() || m_iface->hasStackTrace();
    ui->detailsTabWidget->setVisible(hasAnyDetails);
    if (!hasAnyDetails)
        return;

    const auto hasAllDetails = m_iface->hasArgumentDetails() && m_iface->hasStackTrace();
    ui->detailsTabWidget->tabBar()->setVisible(hasAllDetails);
    if (hasAllDetails)
        return;

    ui->detailsTabWidget->setCurrentWidget(m_iface->hasArgumentDetails() ? ui->argumentTab : ui->stackTraceTab);
}

void PaintAnalyzerWidget::commandContextMenu(QPoint pos)
{
    const auto idx = ui->commandView->indexAt(pos);
    if (!idx.isValid())
        return;

    const auto objectId = idx.data(PaintBufferModelRoles::ObjectIdRole).value<ObjectId>();

    QMenu contextMenu;
    ContextMenuExtension cme(objectId);
    cme.populateMenu(&contextMenu);
    contextMenu.exec(ui->commandView->viewport()->mapToGlobal(pos));
}

void PaintAnalyzerWidget::stackTraceContextMenu(QPoint pos)
{
    const auto idx = ui->stackTraceView->indexAt(pos);
    if (!idx.isValid())
        return;

    const auto loc = idx.sibling(idx.row(), 1).data().value<SourceLocation>();
    if (!loc.isValid())
        return;

    QMenu contextMenu;
    ContextMenuExtension cme;
    cme.setLocation(ContextMenuExtension::ShowSource, loc);
    cme.populateMenu(&contextMenu);
    contextMenu.exec(ui->stackTraceView->viewport()->mapToGlobal(pos));
}
