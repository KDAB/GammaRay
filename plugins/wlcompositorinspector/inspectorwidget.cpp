/*
  inspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

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

#include "inspectorwidget.h"

#include <common/objectbroker.h>

#include <QAbstractItemModel>
#include <QDebug>
#include <QMouseEvent>
#include <QScrollBar>
#include <QStaticText>
#include <QPainter>
#include <QScrollArea>
#include <QClipboard>

#include "ui_inspectorwidget.h"
#include "wlcompositorclient.h"
#include "logview.h"

using namespace GammaRay;

static QObject *wlCompositorClientFactory(const QString &/*name*/, QObject *parent)
{
  return new WlCompositorClient(parent);
}

InspectorWidget::InspectorWidget(QWidget *parent)
               : QWidget(parent)
               , m_ui(new Ui::Widget)
{
    ObjectBroker::registerClientObjectFactoryCallback<WlCompositorInterface *>(wlCompositorClientFactory);
    m_client = ObjectBroker::object<WlCompositorInterface *>();
    m_client->connected();

    m_model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.WaylandCompositorClientsModel"));
    m_ui->setupUi(this);

    m_ui->resourceInfo->setVisible(false);

    auto resourcesModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.WaylandCompositorResourcesModel"));
    m_ui->resourcesView->setModel(resourcesModel);

    m_logView = new LogView(this);
    m_logView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_ui->gridLayout->addWidget(m_logView, 2, 0, 1, 2);
    connect(m_client, &WlCompositorInterface::logMessage, m_logView, &LogView::logMessage);
    connect(m_client, &WlCompositorInterface::resetLog, m_logView, &LogView::reset);

    m_ui->clientsView->setModel(m_model);
    m_ui->clientsView->viewport()->installEventFilter(this);
    connect(m_ui->resourcesView->selectionModel(), &QItemSelectionModel::currentChanged, this, &InspectorWidget::resourceActivated);
    m_ui->resourcesView->viewport()->installEventFilter(this);
}

InspectorWidget::~InspectorWidget()
{
  m_client->disconnected();
}

void InspectorWidget::delayedInit()
{
}

void InspectorWidget::clientActivated(const QModelIndex &index)
{
    m_client->setSelectedClient(index.row());
}

void InspectorWidget::resourceActivated(const QModelIndex &index)
{
    QString text = index.data(Qt::ToolTipRole).toString();
    m_ui->resourceInfo->setText(text);
    m_ui->resourceInfo->setVisible(!text.isEmpty());
}

bool InspectorWidget::eventFilter(QObject *o, QEvent *e)
{
    switch (e->type()) {
        case QEvent::MouseButtonRelease: {
            auto *me = static_cast<QMouseEvent *>(e);
            if (o == m_ui->clientsView->viewport()) {
                clientActivated(m_ui->clientsView->indexAt(me->pos()));
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
