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

using namespace GammaRay;

MessageHandlerWidget::MessageHandlerWidget(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::MessageHandlerWidget)
{
  ui->setupUi(this);

  proxy = new QSortFilterProxyModel(this);
  proxy->setSourceModel(ObjectBroker::model("com.kdab.GammaRay.MessageModel"));
  ui->messageSearchLine->setProxy(proxy);
  ui->messageView->setModel(proxy);
  ui->messageView->setIndentation(0);
  ui->messageView->setSortingEnabled(true);

  m_backtraceModel.reset(new BacktraceModel);
  ui->backtraceView->setModel(m_backtraceModel.data());

  QItemSelectionModel *selectionModel = ui->messageView->selectionModel();
  connect(selectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(selectionChanged(QModelIndex,QModelIndex)));
}

MessageHandlerWidget::~MessageHandlerWidget()
{
}



void MessageHandlerWidget::selectionChanged(const QModelIndex &current, const QModelIndex &previous)
{
  Q_UNUSED(previous);
  m_backtraceModel->setBacktrace(proxy->data(current, Qt::UserRole).toStringList());
  for (int i = 0; i < m_backtraceModel->columnCount(); ++i)
    ui->backtraceView->resizeColumnToContents(i);
}

