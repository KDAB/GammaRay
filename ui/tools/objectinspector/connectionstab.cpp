/*
  connectionstab.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "connectionstab.h"
#include "ui_connectionstab.h"
#include "connectionsclientproxymodel.h"

#include <ui/propertywidget.h>
#include <ui/searchlinecontroller.h>

#include <common/objectbroker.h>
#include <common/tools/objectinspector/connectionsextensioninterface.h>
#include <common/tools/objectinspector/connectionsmodelroles.h>

#include <QDebug>
#include <QMenu>

using namespace GammaRay;

ConnectionsTab::ConnectionsTab(PropertyWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ConnectionsTab)
{
    m_interface = ObjectBroker::object<ConnectionsExtensionInterface *>(
        parent->objectBaseName() + ".connectionsExtension");

    ui->setupUi(this);
    ui->inboundView->header()->setObjectName("inboundViewHeader");
    ui->outboundView->header()->setObjectName("outboundViewHeader");

    QSortFilterProxyModel *proxy = new ConnectionsClientProxyModel(this);
    proxy->setDynamicSortFilter(true);
    proxy->setSourceModel(ObjectBroker::model(parent->objectBaseName() + ".inboundConnections"));
    ui->inboundView->setModel(proxy);
    ui->inboundView->sortByColumn(0, Qt::AscendingOrder);
    new SearchLineController(ui->inboundSearchLine, proxy);
    connect(ui->inboundView, &QWidget::customContextMenuRequested,
            this, &ConnectionsTab::inboundContextMenu);

    proxy = new ConnectionsClientProxyModel(this);
    proxy->setDynamicSortFilter(true);
    proxy->setSourceModel(ObjectBroker::model(parent->objectBaseName() + ".outboundConnections"));
    ui->outboundView->setModel(proxy);
    ui->outboundView->sortByColumn(0, Qt::AscendingOrder);
    new SearchLineController(ui->outboundSearchLine, proxy);
    connect(ui->outboundView, &QWidget::customContextMenuRequested,
            this, &ConnectionsTab::outboundContextMenu);
}

ConnectionsTab::~ConnectionsTab() = default;

static int mapToSourceRow(const QModelIndex &index)
{
    QModelIndex i = index;
    while (const QAbstractProxyModel *proxy = qobject_cast<const QAbstractProxyModel *>(i.model()))
        i = proxy->mapToSource(i);
    return i.row();
}

void ConnectionsTab::inboundContextMenu(const QPoint &pos)
{
    const QModelIndex index = ui->inboundView->currentIndex();
    if (!index.isValid() || index.data(ConnectionsModelRoles::ActionRole).toInt() == 0)
        return;

    QMenu menu;
    menu.addAction(tr("Go to sender"));
    if (menu.exec(ui->inboundView->viewport()->mapToGlobal(pos)))
        m_interface->navigateToSender(mapToSourceRow(index));
}

void ConnectionsTab::outboundContextMenu(const QPoint &pos)
{
    const QModelIndex index = ui->outboundView->currentIndex();
    if (!index.isValid() || index.data(ConnectionsModelRoles::ActionRole).toInt() == 0)
        return;

    QMenu menu;
    menu.addAction(tr("Go to receiver"));
    if (menu.exec(ui->outboundView->viewport()->mapToGlobal(pos)))
        m_interface->navigateToReceiver(mapToSourceRow(index));
}
