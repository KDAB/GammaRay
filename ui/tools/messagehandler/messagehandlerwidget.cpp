/*
  messagehandlerwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "messagehandlerwidget.h"
#include "ui_messagehandlerwidget.h"

#include <common/endpoint.h>
#include <common/objectbroker.h>

#include <QSortFilterProxyModel>
#include <QTreeView>


#include <core/tools/messagehandler/messagehandlerinterface.h>

using namespace GammaRay;

MessageHandlerWidget::MessageHandlerWidget(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::MessageHandlerWidget)
{
  ui->setupUi(this);

  m_handler = ObjectBroker::object<MessageHandlerInterface*>();

  QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
  proxy->setSourceModel(ObjectBroker::model("com.kdab.GammaRay.MessageModel"));
  ui->messageSearchLine->setProxy(proxy);
  ui->messageView->setModel(proxy);
  ui->messageView->setIndentation(0);
  ui->messageView->setSortingEnabled(true);

  ui->backtraceView->setModel(ObjectBroker::model("com.kdab.GammaRay.BacktraceModel"));

  QItemSelectionModel *selectionModel = ui->messageView->selectionModel();
  connect(selectionModel, SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(currentRowChanged(QModelIndex,QModelIndex)));
}

MessageHandlerWidget::~MessageHandlerWidget()
{
}



void MessageHandlerWidget::currentRowChanged(const QModelIndex &current, const QModelIndex &previous)
{
  Q_UNUSED(previous);
  m_handler->selectMessage(current.row());
  int columnCount = ui->backtraceView->model()->columnCount();
  for (int i = 0; i < columnCount; ++i)
    ui->backtraceView->resizeColumnToContents(i);
}

