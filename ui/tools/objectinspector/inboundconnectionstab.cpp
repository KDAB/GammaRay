/*
  inboundconnectionstab.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#include "inboundconnectionstab.h"
#include "ui_inboundconnectionstab.h"
#include "propertywidget.h"

#include "common/objectbroker.h"

#include <QSortFilterProxyModel>

using namespace GammaRay;

InboundConnectionsTab::InboundConnectionsTab(PropertyWidget *parent) : QWidget(parent),
  m_ui(new Ui_InboundConnectionsTab)
{
  m_ui->setupUi(this);
  connect(parent, SIGNAL(objectBaseNameChanged(QString)), this, SLOT(setObjectBaseName(QString)));
}

InboundConnectionsTab::~InboundConnectionsTab()
{
}

void InboundConnectionsTab::setObjectBaseName(const QString &baseName)
{
  QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(ObjectBroker::model(baseName + '.' + "inboundConnections"));
  m_ui->inboundConnectionView->setModel(proxy);
  m_ui->inboundConnectionView->sortByColumn(0, Qt::AscendingOrder);
  m_ui->inboundConnectionSearchLine->setProxy(proxy);
}
