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

#include "ui_inspectorwidget.h"
#include "wlcompositorclient.h"

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

    m_model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.WaylandCompositorClientsModel"));
    m_ui->setupUi(this);

    auto resourcesModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.WaylandCompositorResourcesModel"));
    m_ui->resourcesView->setModel(resourcesModel);

    m_ui->clientsView->setModel(m_model);
    connect(m_ui->clientsView, &QAbstractItemView::clicked, this, &InspectorWidget::clientActivated);
    connect(m_ui->resourcesView, &QAbstractItemView::clicked, this, &InspectorWidget::resourceActivated);
}

InspectorWidget::~InspectorWidget()
{
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
    QString model = index.data(Qt::DisplayRole).toString();
}
