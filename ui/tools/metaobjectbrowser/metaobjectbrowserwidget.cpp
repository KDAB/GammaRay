/*
  metaobjectbrowserwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "metaobjectbrowserwidget.h"
#include "metaobjecttreeclientproxymodel.h"

#include <ui/propertywidget.h>
#include <ui/deferredtreeview.h>
#include <ui/searchlinecontroller.h>

#include <common/endpoint.h>
#include <common/objectbroker.h>

#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>

using namespace GammaRay;

MetaObjectBrowserWidget::MetaObjectBrowserWidget(QWidget *parent)
    : QWidget(parent)
    , m_stateManager(this)
{
    setObjectName("MetaObjectBrowserWidget");

    auto model
        = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.MetaObjectBrowserTreeModel"));
    auto proxy = new MetaObjectTreeClientProxyModel(this);
    proxy->setSourceModel(model);

    m_treeView = new DeferredTreeView(this);
    m_treeView->header()->setObjectName("metaObjectViewHeader");
    m_treeView->setStretchLastSection(false);
    m_treeView->setExpandNewContent(true);
    m_treeView->setDeferredResizeMode(0, QHeaderView::Stretch);
    m_treeView->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    m_treeView->setDeferredResizeMode(2, QHeaderView::ResizeToContents);
    m_treeView->setDeferredResizeMode(3, QHeaderView::ResizeToContents);
    m_treeView->setDeferredResizeMode(4, QHeaderView::ResizeToContents);
    m_treeView->setUniformRowHeights(true);
    m_treeView->setModel(proxy);
    m_treeView->setSelectionModel(ObjectBroker::selectionModel(proxy));
    m_treeView->sortByColumn(0, Qt::AscendingOrder);
    connect(m_treeView->selectionModel(), &QItemSelectionModel::selectionChanged, this,
            &MetaObjectBrowserWidget::selectionChanged);

    auto objectSearchLine = new QLineEdit(this);
    new SearchLineController(objectSearchLine, proxy);

    auto *propertyWidget = new PropertyWidget(this);
    m_propertyWidget = propertyWidget;
    m_propertyWidget->setObjectBaseName(QStringLiteral("com.kdab.GammaRay.MetaObjectBrowser"));

    auto *vbox = new QVBoxLayout;
    vbox->addWidget(objectSearchLine);
    vbox->addWidget(m_treeView);

    auto *hbox = new QHBoxLayout(this);
    hbox->addLayout(vbox);
    hbox->addWidget(propertyWidget);

    connect(m_propertyWidget, &PropertyWidget::tabsUpdated, this, &MetaObjectBrowserWidget::propertyWidgetTabsChanged);

	Endpoint::instance()->invokeObject(QStringLiteral("com.kdab.GammaRay.MetaObjectBrowser"), "rescanMetaTypes");
}

void MetaObjectBrowserWidget::selectionChanged(const QItemSelection &selection)
{
    if (selection.isEmpty())
        return;

    m_treeView->scrollTo(selection.first().topLeft()); // in case of remote changes
}

void MetaObjectBrowserWidget::propertyWidgetTabsChanged()
{
    m_stateManager.saveState();
    m_stateManager.reset();
}
