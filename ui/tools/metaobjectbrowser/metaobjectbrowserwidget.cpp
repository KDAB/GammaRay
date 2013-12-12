/*
  metaobjectbrowserwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "metaobjectbrowserwidget.h"
#include "propertywidget.h"
#include "deferredresizemodesetter.h"
#include <deferredtreeviewconfiguration.h>

#include "kde/kfilterproxysearchline.h"
#include "kde/krecursivefilterproxymodel.h"

#include <common/objectbroker.h>

#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTreeView>

using namespace GammaRay;

MetaObjectBrowserWidget::MetaObjectBrowserWidget(QWidget *parent)
  : QWidget(parent)
{
  QAbstractItemModel *model = ObjectBroker::model("com.kdab.GammaRay.MetaObjectModel");

  QSortFilterProxyModel *objectFilter = new KRecursiveFilterProxyModel(this);
  objectFilter->setSourceModel(model);
  objectFilter->setDynamicSortFilter(true);

  QTreeView *treeView = new QTreeView(this);
  treeView->setUniformRowHeights(true);
  treeView->setModel(objectFilter);
  new DeferredResizeModeSetter(treeView->header(), 0, QHeaderView::Stretch);
  treeView->setSortingEnabled(true);
  treeView->setSelectionModel(ObjectBroker::selectionModel(objectFilter));

  KFilterProxySearchLine *objectSearchLine = new KFilterProxySearchLine(this);
  objectSearchLine->setProxy(objectFilter);

  PropertyWidget *propertyWidget = new PropertyWidget(this);
  m_propertyWidget = propertyWidget;
  m_propertyWidget->setObjectBaseName("com.kdab.GammaRay.MetaObjectBrowser");

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addWidget(objectSearchLine);
  vbox->addWidget(treeView);

  QHBoxLayout *hbox = new QHBoxLayout(this);
  hbox->addLayout(vbox);
  hbox->addWidget(propertyWidget);

  // init widget
  new DeferredTreeViewConfiguration(treeView);
  treeView->sortByColumn(0, Qt::AscendingOrder);
}

