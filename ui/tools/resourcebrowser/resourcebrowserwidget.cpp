/*
  resourcebrowserwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "resourcebrowserwidget.h"

#include "ui_resourcebrowserwidget.h"
#include "resourcebrowserclient.h"
#include "clientresourcemodel.h"

#include <deferredtreeviewconfiguration.h>
#include <3rdparty/qt/resourcemodel.h>
#include <common/objectbroker.h>

#include <QDebug>
#include <QFileInfo>
#include <QTimer>

using namespace GammaRay;

static QObject* createResourceBrowserClient(const QString & /*name*/, QObject *parent)
{
  return new ResourceBrowserClient(parent);
}

ResourceBrowserWidget::ResourceBrowserWidget(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::ResourceBrowserWidget)
  , m_timer(new QTimer(this))
  , m_interface(0)
{
  ObjectBroker::registerClientObjectFactoryCallback<ResourceBrowserInterface*>(createResourceBrowserClient);
  m_interface = ObjectBroker::object<ResourceBrowserInterface*>();
  connect(m_interface, SIGNAL(resourceDeselected()), this, SLOT(resourceDeselected()));
  connect(m_interface, SIGNAL(resourceSelected(QPixmap)), this, SLOT(resourceSelected(QPixmap)));
  connect(m_interface, SIGNAL(resourceSelected(QByteArray)), this, SLOT(resourceSelected(QByteArray)));

  ui->setupUi(this);
  ClientResourceModel* model = new ClientResourceModel(this);
  model->setSourceModel(ObjectBroker::model("com.kdab.GammaRay.ResourceModel"));
  ui->treeView->setModel(model);
  ui->treeView->setSelectionModel(ObjectBroker::selectionModel(ui->treeView->model()));
  ui->searchLine->setProxy(model);

  DeferredTreeViewConfiguration *config = new DeferredTreeViewConfiguration(ui->treeView);
  config->hideColumn(3);
  connect(ui->treeView->model(), SIGNAL(rowsInserted(QModelIndex,int,int)),
          SLOT(rowsInserted()));

  ui->resourceLabel->setText(tr("Select a Resource to Preview"));
  ui->stackedWidget->setCurrentWidget(ui->contentLabelPage);

  m_timer->setInterval(100);
  m_timer->setSingleShot(true);
  connect(m_timer, SIGNAL(timeout()), SLOT(setupLayout()));
  m_timer->start();
}

ResourceBrowserWidget::~ResourceBrowserWidget()
{
}

void ResourceBrowserWidget::rowsInserted()
{
  m_timer->start();
}

void ResourceBrowserWidget::setupLayout()
{
  // now we can assume the model is filled properly and can adjust the tree view column sizes
  for(int i = 0; i < 3; ++i) {
    ui->treeView->resizeColumnToContents(i);
  }

  // now the view was setup properly and we can mess with the splitter to resize
  // the widgets for nicer display

  int viewWidth = ui->treeView->columnWidth(0) +
                  ui->treeView->columnWidth(1) +
                  ui->treeView->columnWidth(2) +
                  ui->treeView->contentsMargins().left() +
                  ui->treeView->contentsMargins().right() + 25;
  const int totalWidth = ui->splitter->width();
  const int minPreviewWidth = 150;
  if (totalWidth > viewWidth + minPreviewWidth) {
    ui->splitter->setSizes(QList<int>() << viewWidth << (totalWidth - viewWidth));
    ui->splitter->setStretchFactor(1, 3);
  }
}

void ResourceBrowserWidget::resourceDeselected()
{
  ui->resourceLabel->setText(tr("Select a Resource to Preview"));
  ui->stackedWidget->setCurrentWidget(ui->contentLabelPage);
}

void ResourceBrowserWidget::resourceSelected(const QPixmap &pixmap)
{
  ui->resourceLabel->setPixmap(pixmap);
  ui->stackedWidget->setCurrentWidget(ui->contentLabelPage);
}

void ResourceBrowserWidget::resourceSelected(const QByteArray &contents)
{
  //TODO: make encoding configurable
  ui->textBrowser->setText(contents);
  ui->stackedWidget->setCurrentWidget(ui->contentTextPage);
}

