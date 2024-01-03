/*
  inspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "inspectorwidget.h"

#include <ui/contextmenuextension.h>
#include <ui/remoteviewwidget.h>

#include <common/objectbroker.h>
#include <common/objectid.h>

#include <QAbstractItemModel>
#include <QDebug>
#include <QMenu>
#include <QMouseEvent>
#include <QScrollBar>
#include <QStaticText>
#include <QPainter>
#include <QScrollArea>
#include <QClipboard>

#include "ui_inspectorwidget.h"
#include "wlcompositorclient.h"
#include "logview.h"
#include "clientsmodel.h" // only for extra roles

using namespace GammaRay;

class RemoteSurfaceView : public RemoteViewWidget
{
public:
    explicit RemoteSurfaceView(QWidget *parent)
        : RemoteViewWidget(parent)
    {
        setName(QStringLiteral("com.kdab.GammaRay.WaylandCompositorSurfaceView"));
    }
};

static QObject *wlCompositorClientFactory(const QString & /*name*/, QObject *parent)
{
    return new WlCompositorClient(parent);
}

InspectorWidget::InspectorWidget(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::InspectorWidget)
{
    ObjectBroker::registerClientObjectFactoryCallback<WlCompositorInterface *>(wlCompositorClientFactory);
    m_client = ObjectBroker::object<WlCompositorInterface *>();
    m_client->connected();

    m_ui->setupUi(this);
    m_ui->resourceInfo->setVisible(false);

    auto resourcesModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.WaylandCompositorResourcesModel"));
    m_ui->resourcesView->setModel(resourcesModel);

    m_logView = new LogView(this);
    m_logView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_ui->gridLayout->addWidget(m_logView, 2, 0, 1, 2);
    connect(m_client, &WlCompositorInterface::logMessage, m_logView, &LogView::logMessage);
    connect(m_client, &WlCompositorInterface::resetLog, m_logView, &LogView::reset);
    connect(m_client, &WlCompositorInterface::setLoggingClient, m_logView, &LogView::setLoggingClient);

    m_model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.WaylandCompositorClientsModel"));
    auto clientSelectionModel = ObjectBroker::selectionModel(m_model);
    connect(clientSelectionModel, &QItemSelectionModel::selectionChanged, this, &InspectorWidget::clientSelected);
    m_ui->clientsView->setModel(m_model);
    m_ui->clientsView->setSelectionModel(clientSelectionModel);
    m_ui->clientsView->viewport()->installEventFilter(this);
    connect(m_ui->clientsView, &QTreeView::customContextMenuRequested, this, &InspectorWidget::clientContextMenu);

    connect(m_ui->resourcesView->selectionModel(), &QItemSelectionModel::currentChanged, this, &InspectorWidget::resourceActivated);
    m_ui->resourcesView->viewport()->installEventFilter(this);

    auto *surfaceView = new RemoteSurfaceView(this);
    m_ui->gridLayout->addWidget(surfaceView, 1, 0, 1, 1);
}

InspectorWidget::~InspectorWidget()
{
    m_client->disconnected();
}

void InspectorWidget::delayedInit()
{
}

void InspectorWidget::clientSelected(const QItemSelection &selection)
{
    if (selection.isEmpty()) {
        m_client->setSelectedClient(-1);
        return;
    }

    const auto index = selection.at(0).topLeft();
    m_client->setSelectedClient(index.row());
}

void InspectorWidget::clientContextMenu(QPoint pos)
{
    auto index = m_ui->clientsView->indexAt(pos);
    if (!index.isValid())
        return;
    index = index.sibling(index.row(), 0);

    const auto objectId = index.data(ClientsModel::ObjectIdRole).value<ObjectId>();
    if (objectId.isNull())
        return;

    QMenu menu;
    ContextMenuExtension ext(objectId);
    ext.populateMenu(&menu);
    menu.exec(m_ui->clientsView->viewport()->mapToGlobal(pos));
}

void InspectorWidget::resourceActivated(const QModelIndex &index)
{
    QString text = index.data(Qt::ToolTipRole).toString();
    m_client->setSelectedResource(index.data(Qt::UserRole + 2).toUInt());
    m_ui->resourceInfo->setText(text);
    m_ui->resourceInfo->setVisible(!text.isEmpty());
}

bool InspectorWidget::eventFilter(QObject *o, QEvent *e)
{
    switch (e->type()) {
    case QEvent::MouseButtonRelease: {
        auto *me = static_cast<QMouseEvent *>(e);
        if (o == m_ui->clientsView->viewport()) {
            const auto idx = m_ui->clientsView->indexAt(me->pos());
            if (!idx.isValid())
                m_ui->clientsView->selectionModel()->clear();
        } else {
            QModelIndex index = m_ui->resourcesView->indexAt(me->pos());
            if (!index.isValid()) {
                m_ui->resourcesView->setCurrentIndex(index);
            }
        }
        return false;
    }
    default:
        break;
    }
    return QWidget::eventFilter(o, e);
}
