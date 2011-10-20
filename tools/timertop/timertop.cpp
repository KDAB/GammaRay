/*
  timertop.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "timertop.h"

#include "ui_timertop.h"
#include "timermodel.h"

#include <QSortFilterProxyModel>

using namespace GammaRay;

// thread saftey!
// timer events
// generalize for signal profiling for every signal
// retrieve receiver name from connection model

// columns: wakeups, time / wakeup, max time, objectname, receiver
//    -> averaged over the last x seconds. X being configurable, plus clear button
// objectname: if not set, use child^3 of xyz
// active (repeating, singleshot)
// interval
// countdown (refresh every 1 second)
// slot: slotXYZ() and 3 others (shown in tooltip)
// flash delegate when timer triggered
// heat map?
// backgroudn color: either status (runing, stopped, singleshot) or frequency
// move callback handling to probe interface

// clear timeoutEvents when adding new ones

// timeout -> it needs to be in list already!

// filter out timers specific to gammaray (over global blacklist?)

// test timer added/removed


// put functioncalltimer into own file
// uncomment dump method
// filter out gammaray timers with qsortfilterproxymodel
// protect against timer deletion
// make the connection interface generic and add to probeinterface

// update every x milliseconds and trigger update on timeout()

// remove unable to find timer warning
// add a property widget

TimerTop::TimerTop(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent),
    ui(new Ui::TimerTop)
{  
  Q_UNUSED(probe);
  ui->setupUi(this);
  TimerModel::instance()->setProbeInterface(probe);
  QSortFilterProxyModel *sortProxy = new QSortFilterProxyModel(this);
  sortProxy->setSourceModel(TimerModel::instance());
  sortProxy->setDynamicSortFilter(true);
  ui->timerView->setModel(sortProxy);
  ui->timerView->sortByColumn(TimerModel::WakeupsPerSecRole - TimerModel::FirstRole - 1, Qt::DescendingOrder);
}

#include "timertop.moc"
