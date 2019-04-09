/*
  timertopwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Thomas McGuire <thomas.mcguire@kdab.com>

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

#include "timertopwidget.h"
#include "ui_timertopwidget.h"
#include "timermodel.h"
#include "timertopclient.h"
#include "clienttimermodel.h"

#include <ui/contextmenuextension.h>
#include <ui/searchlinecontroller.h>

#include <common/objectbroker.h>

#include <QMenu>
#include <QSortFilterProxyModel>
#include <QTimer>

using namespace GammaRay;

static QObject *createTimerTopClient(const QString & /*name*/, QObject *parent)
{
    return new TimerTopClient(parent);
}

TimerTopWidget::TimerTopWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TimerTopWidget)
    , m_stateManager(this)
{
    ui->setupUi(this);

    ObjectBroker::registerClientObjectFactoryCallback<TimerTopInterface *>(
        createTimerTopClient);

    m_interface = ObjectBroker::object<TimerTopInterface *>();

    ui->timerView->header()->setObjectName("timerViewHeader");
    ui->timerView->setDeferredResizeMode(0, QHeaderView::Stretch);
    ui->timerView->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    ui->timerView->setDeferredResizeMode(2, QHeaderView::ResizeToContents);
    ui->timerView->setDeferredResizeMode(3, QHeaderView::ResizeToContents);
    ui->timerView->setDeferredResizeMode(4, QHeaderView::ResizeToContents);
    ui->timerView->setDeferredResizeMode(5, QHeaderView::ResizeToContents);
    connect(ui->timerView, &QWidget::customContextMenuRequested, this, &TimerTopWidget::contextMenu);
    connect(ui->clearTimers, &QAbstractButton::clicked, m_interface, &TimerTopInterface::clearHistory);

    auto * const sortModel = new ClientTimerModel(this);
    sortModel->setSourceModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TimerModel")));
    sortModel->setDynamicSortFilter(true);
    ui->timerView->setModel(sortModel);
    ui->timerView->setSelectionModel(ObjectBroker::selectionModel(sortModel));

    new SearchLineController(ui->timerViewFilter, sortModel);

    ui->timerView->sortByColumn(TimerModel::WakeupsPerSecColumn, Qt::DescendingOrder);
}

TimerTopWidget::~TimerTopWidget() = default;

void TimerTopWidget::contextMenu(QPoint pos)
{
    auto index = ui->timerView->indexAt(pos);
    if (!index.isValid())
        return;
    index = index.sibling(index.row(), 0);

    const auto objectId = index.data(ObjectModel::ObjectIdRole).value<ObjectId>();
    if (objectId.isNull())
        return;

    QMenu menu;
    ContextMenuExtension ext(objectId);
    ext.setLocation(ContextMenuExtension::Creation, index.data(ObjectModel::CreationLocationRole).value<SourceLocation>());
    ext.setLocation(ContextMenuExtension::Declaration, index.data(ObjectModel::DeclarationLocationRole).value<SourceLocation>());
    ext.populateMenu(&menu);
    menu.exec(ui->timerView->viewport()->mapToGlobal(pos));
}
