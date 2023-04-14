/*
  qtivisupportwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "qtivisupportwidget.h"
#include "qtivipropertyclientmodel.h"
#include "qtiviconstrainedvaluedelegate.h"

#include <common/endpoint.h>
#include <common/objectbroker.h>
#include <common/objectmodel.h>
#include <ui/contextmenuextension.h>
#include <ui/searchlinecontroller.h>
#include <ui/deferredtreeview.h>

#include <QVBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QMenu>
#include <QTreeView>

using namespace GammaRay;

QtIVIWidget::QtIVIWidget(QWidget *parent)
    : QWidget(parent)
    , m_stateManager(this)
{
    setObjectName("QtIVIWidget");
    QAbstractItemModel *propertyModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.PropertyModel"));
    auto *clientModel = new QtIviPropertyClientModel(this);
    clientModel->setSourceModel(propertyModel);
    QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(clientModel);

    connect(propertyModel, &QAbstractItemModel::destroyed, this, [clientModel]() {
        clientModel->setSourceModel(nullptr);
    });

    auto *vbox = new QVBoxLayout(this);

    auto *searchLineEdit = new QLineEdit(this);
    vbox->addWidget(searchLineEdit);
    new SearchLineController(searchLineEdit, clientModel);

    m_objectTreeView = new DeferredTreeView(this);
    m_objectTreeView->header()->setObjectName("objectTreeViewHeader");
    m_objectTreeView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    m_objectTreeView->setDeferredResizeMode(1, QHeaderView::Interactive);
    m_objectTreeView->setDeferredResizeMode(2, QHeaderView::ResizeToContents);
    m_objectTreeView->setDeferredResizeMode(3, QHeaderView::ResizeToContents);
    m_objectTreeView->setDeferredResizeMode(4, QHeaderView::ResizeToContents);
    m_objectTreeView->setExpandNewContent(true);
    vbox->addWidget(m_objectTreeView);

    m_objectTreeView->setSortingEnabled(true);
    m_objectTreeView->setModel(clientModel);
    m_objectTreeView->setItemDelegateForColumn(1, new QtIviConstrainedValueDelegate(this));
    m_objectTreeView->setSelectionModel(selectionModel);
    connect(selectionModel,
            &QItemSelectionModel::selectionChanged,
            this, &QtIVIWidget::objectSelected);

    setContextMenuPolicy(Qt::CustomContextMenu);
    m_objectTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_objectTreeView, &QWidget::customContextMenuRequested, this, &QtIVIWidget::contextMenu);
}

void QtIVIWidget::objectSelected(const QItemSelection &selection)
{
    QModelIndex index;
    if (!selection.isEmpty())
        index = selection.first().topLeft();

    if (index.isValid()) {
        m_objectTreeView->scrollTo(index);
    }
}

void QtIVIWidget::contextMenu(QPoint pos)
{
    QModelIndex index = m_objectTreeView->indexAt(pos);
    if (!index.isValid())
        return;
    index = index.sibling(index.row(), 0);
    const ObjectId objectId = index.data(ObjectModel::ObjectIdRole).value<ObjectId>();
    if (objectId.isNull())
        return;

    QMenu menu;
    ContextMenuExtension ext(objectId);
    ext.populateMenu(&menu);
    menu.exec(m_objectTreeView->viewport()->mapToGlobal(pos));
}
