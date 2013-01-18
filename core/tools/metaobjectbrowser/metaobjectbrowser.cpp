/*
  metaobjectbrowser.cpp

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

#include "metaobjectbrowser.h"
#include "metaobjecttreemodel.h"
#include "probe.h"
#include "propertywidget.h"

#include "kde/kfilterproxysearchline.h"
#include "kde/krecursivefilterproxymodel.h"

#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QTreeView>

using namespace GammaRay;

MetaObjectBrowser::MetaObjectBrowser(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent)
{
  Q_UNUSED(probe);
  QAbstractItemModel *model = Probe::instance()->metaObjectModel();

  QSortFilterProxyModel *objectFilter = new KRecursiveFilterProxyModel(this);
  objectFilter->setSourceModel(model);
  objectFilter->setDynamicSortFilter(true);

  QTreeView *treeView = new QTreeView(this);
  treeView->setModel(objectFilter);
  treeView->header()->setResizeMode(0, QHeaderView::Stretch);
  treeView->setSortingEnabled(true);

  KFilterProxySearchLine *objectSearchLine = new KFilterProxySearchLine(this);
  objectSearchLine->setProxy(objectFilter);
  connect(treeView->selectionModel(),
          SIGNAL(currentChanged(QModelIndex,QModelIndex)),
          SLOT(objectSelected(QModelIndex)));

  PropertyWidget *propertyWidget = new PropertyWidget(this);
  propertyWidget->setMetaObject(0); // init
  m_propertyWidget = propertyWidget;

  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->addWidget(objectSearchLine);
  vbox->addWidget(treeView);

  QHBoxLayout *hbox = new QHBoxLayout(this);
  hbox->addLayout(vbox);
  hbox->addWidget(propertyWidget);

  // init widget
  treeView->sortByColumn(0, Qt::AscendingOrder);
  const QModelIndex firstIndex = objectFilter->index(0, 0);
  treeView->expand(firstIndex);
}

void MetaObjectBrowser::objectSelected(const QModelIndex &index)
{
  if (index.isValid()) {
    const QMetaObject *metaObject =
      index.data(MetaObjectTreeModel::MetaObjectRole).value<const QMetaObject*>();
    m_propertyWidget->setMetaObject(metaObject);
  } else {
    m_propertyWidget->setMetaObject(0);
  }
}

#include "metaobjectbrowser.moc"
