/*
  eventmonitorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Tim Henning <tim.henning@kdab.com>

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

#include "eventmonitorwidget.h"
#include "ui_eventmonitorwidget.h"

#include "eventmodelroles.h"
#include "eventmonitorclient.h"

#include <ui/clientpropertymodel.h>
#include <ui/propertyeditor/propertyeditordelegate.h>
#include <ui/searchlinecontroller.h>

#include <common/objectbroker.h>

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

    QAbstractItemModel * const eventModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.EventModel"));
    new SearchLineController(ui->eventSearchLine, eventModel);
    ui->eventTree->setDeferredResizeMode(EventModelColumn::Time, QHeaderView::ResizeToContents);
    ui->eventTree->setModel(eventModel);
    ui->eventTree->setSelectionModel(ObjectBroker::selectionModel(ui->eventTree->model()));

    connect(ui->pauseButton, &QAbstractButton::toggled, this, &EventMonitorWidget::pauseAndResume);
    connect(ui->clearButton, &QAbstractButton::pressed, m_interface, &EventMonitorInterface::clearHistory);

    auto clientPropModel = new ClientPropertyModel(this);
    clientPropModel->setSourceModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.EventPropertyModel")));
    ui->eventInspector->setModel(clientPropModel);
    ui->eventInspector->setItemDelegate(new PropertyEditorDelegate(this));

    QAbstractItemModel * const eventTypeModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.EventTypeModel"));
    ui->eventTypeTree->setModel(eventTypeModel);
}

EventMonitorWidget::~EventMonitorWidget()
{
    delete ui;
}

void EventMonitorWidget::pauseAndResume(bool pause)
{
    m_interface->setIsPaused(pause);
}
