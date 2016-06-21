/*
  timertopwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <common/objectbroker.h>

#include <QSortFilterProxyModel>
#include <QTimer>

using namespace GammaRay;

TimerTopWidget::TimerTopWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TimerTopWidget)
    , m_stateManager(this)
    , m_updateTimer(new QTimer(this))
{
    ui->setupUi(this);

    ui->timerView->header()->setObjectName("timerViewHeader");
    ui->timerView->setDeferredResizeMode(0, QHeaderView::Stretch);
    ui->timerView->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    ui->timerView->setDeferredResizeMode(2, QHeaderView::ResizeToContents);
    ui->timerView->setDeferredResizeMode(3, QHeaderView::ResizeToContents);
    ui->timerView->setDeferredResizeMode(4, QHeaderView::ResizeToContents);
    ui->timerView->setDeferredResizeMode(5, QHeaderView::ResizeToContents);

    QSortFilterProxyModel * const sortModel = new QSortFilterProxyModel(this);
    sortModel->setSourceModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TimerModel")));
    sortModel->setDynamicSortFilter(true);
    ui->timerView->setModel(sortModel);

    ui->timerView->sortByColumn(TimerModel::WakeupsPerSecRole - TimerModel::FirstRole - 1,
                                Qt::DescendingOrder);
}

TimerTopWidget::~TimerTopWidget()
{
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(TimerTopUiFactory)
#endif
