/*
  eventmonitorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tim Henning <tim.henning@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "eventmonitorwidget.h"
#include "ui_eventmonitorwidget.h"

#include "eventmodelroles.h"
#include "eventmonitorclient.h"
#include "eventtypemodel.h"
#include "eventtypeclientproxymodel.h"

#include <ui/clientpropertymodel.h>
#include <ui/contextmenuextension.h>
#include <ui/propertyeditor/propertyeditordelegate.h>
#include <ui/searchlinecontroller.h>

#include <common/objectbroker.h>
#include <common/objectid.h>
#include <common/propertymodel.h>

#include <QMenu>

static QObject *createEventMonitorClient(const QString & /*name*/, QObject *parent)
{
    return new GammaRay::EventMonitorClient(parent);
}

using namespace GammaRay;

EventMonitorWidget::EventMonitorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::EventMonitorWidget)
    , m_interface(nullptr)
{
    ObjectBroker::registerClientObjectFactoryCallback<EventMonitorInterface *>(createEventMonitorClient);
    m_interface = ObjectBroker::object<EventMonitorInterface *>();

    ui->setupUi(this);

    QAbstractItemModel *const eventModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.EventModel"));
    new SearchLineController(ui->eventSearchLine, eventModel);
    ui->eventTree->setDeferredResizeMode(EventModelColumn::Time, QHeaderView::ResizeToContents);
    ui->eventTree->setModel(eventModel);
    ui->eventTree->setSelectionModel(ObjectBroker::selectionModel(ui->eventTree->model()));
    ui->eventTree->header()->setSectionsClickable(false); // we do not want sorting here
    connect(ui->eventTree, &QTreeView::customContextMenuRequested, this, &EventMonitorWidget::eventTreeContextMenu);

    connect(ui->pauseButton, &QAbstractButton::toggled, this, &EventMonitorWidget::pauseAndResume);
    connect(ui->clearButton, &QAbstractButton::pressed, m_interface, &EventMonitorInterface::clearHistory);

    auto clientPropModel = new ClientPropertyModel(this);
    clientPropModel->setSourceModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.EventPropertyModel")));
    ui->eventInspector->setModel(clientPropModel);
    ui->eventInspector->setItemDelegate(new PropertyEditorDelegate(this));
    connect(ui->eventInspector, &QTreeView::customContextMenuRequested, this, &EventMonitorWidget::eventInspectorContextMenu);

    QAbstractItemModel *const eventTypeModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.EventTypeModel"));
    new SearchLineController(ui->typeSearchLine, eventTypeModel);
    EventTypeClientProxyModel *const eventTypeProxyModel = new EventTypeClientProxyModel(this);
    eventTypeProxyModel->setSourceModel(eventTypeModel);
    ui->eventTypeTree->sortByColumn(EventTypeModel::Columns::Type, Qt::AscendingOrder);
    ui->eventTypeTree->setDeferredResizeMode(EventTypeModel::Columns::Type, QHeaderView::Stretch);
    ui->eventTypeTree->setModel(eventTypeProxyModel);

    connect(ui->recordAllButton, &QAbstractButton::pressed, m_interface, &EventMonitorInterface::recordAll);
    connect(ui->recordNoneButton, &QAbstractButton::pressed, m_interface, &EventMonitorInterface::recordNone);
    connect(ui->showAllButton, &QAbstractButton::pressed, m_interface, &EventMonitorInterface::showAll);
    connect(ui->showNoneButton, &QAbstractButton::pressed, m_interface, &EventMonitorInterface::showNone);
}

EventMonitorWidget::~EventMonitorWidget()
{
    delete ui;
}

void EventMonitorWidget::pauseAndResume(bool pause)
{
    m_interface->setIsPaused(pause);
}

void EventMonitorWidget::eventTreeContextMenu(QPoint pos)
{
    auto index = ui->eventTree->indexAt(pos);
    if (!index.isValid())
        return;
    index = index.sibling(index.row(), EventModelColumn::Receiver);

    const auto objectId = index.data(EventModelRole::ReceiverIdRole).value<ObjectId>();
    if (objectId.isNull())
        return;

    QMenu menu;
    ContextMenuExtension ext(objectId);
    ext.populateMenu(&menu);
    menu.exec(ui->eventTree->viewport()->mapToGlobal(pos));
}

void EventMonitorWidget::eventInspectorContextMenu(QPoint pos)
{
    const auto idx = ui->eventInspector->indexAt(pos);
    if (!idx.isValid())
        return;

    const auto actions = idx.data(PropertyModel::ActionRole).toInt();
    const auto objectId = idx.data(PropertyModel::ObjectIdRole).value<ObjectId>();
    ContextMenuExtension ext(objectId);
    const bool canShow = (actions == PropertyModel::NavigateTo && !objectId.isNull())
        || ext.discoverPropertySourceLocation(ContextMenuExtension::GoTo, idx);
    if (!canShow)
        return;

    QMenu contextMenu;
    ext.populateMenu(&contextMenu);
    contextMenu.exec(ui->eventInspector->viewport()->mapToGlobal(pos));
}
