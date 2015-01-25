/*
  timertopwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Thomas McGuire <thomas.mcguire@kdab.com>

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
#include <ui/deferredresizemodesetter.h>

#include <QSortFilterProxyModel>
#include <QTimer>

using namespace GammaRay;

TimerTopWidget::TimerTopWidget(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::TimerTopWidget),
    m_updateTimer(new QTimer(this))
{
  ui->setupUi(this);
  QSortFilterProxyModel * const sortModel = new QSortFilterProxyModel(this);
  sortModel->setSourceModel(ObjectBroker::model("com.kdab.GammaRay.TimerModel"));
  sortModel->setDynamicSortFilter(true);
  ui->timerView->setModel(sortModel);
  ui->timerView->sortByColumn(TimerModel::WakeupsPerSecRole - TimerModel::FirstRole - 1,
                              Qt::DescendingOrder);

  new DeferredResizeModeSetter(ui->timerView->header(), 0, QHeaderView::ResizeToContents);
  new DeferredResizeModeSetter(ui->timerView->header(), 1, QHeaderView::ResizeToContents);
}

TimerTopWidget::~TimerTopWidget()
{
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(TimerTopUiFactory)
#endif
