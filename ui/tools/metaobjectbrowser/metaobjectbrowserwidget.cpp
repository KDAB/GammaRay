/*
  metaobjectbrowserwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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

    auto model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.MetaObjectBrowserTreeModel"));
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
