/*
  timertop.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "timermodel.h"
#include "ui_timertop.h"

#include "include/probeinterface.h"

#include <QtPlugin>

using namespace GammaRay;

//
// TODO list for timer plugin
//

// BUG: Some QTimers appear as free timers
// BUG: QTimeLine class name not shown
// BUG: No thread safety yet
//          Related: Protect against timer deletion
// BUG: Sorting in the view doesn't work well
// BUG: Only top-level timers are shown (bug in probe)

// ! Delete old free timers
// ! Wakeup time for QTimers
// ! Add button to view object info
// ! Test timer added/removed at runtime
// Automatic column resizing
// Filter line edit
// Buttons to kill or slow down timer and start timer
// Add clear button that removes all free timers and resets statistics for normal timers
// Add big fat "total wakeups / sec" label to the status bar
// Retrieve receiver name from connection model
//     Add to view as column: receivers: slotXYZ and 3 others (shown in tooltip)
// Move signal hook to probe interface
//     Then maybe add general signal profiler plugin, or even visualization
// Flash delegate when timer triggered
// Color cell in view redish, depending on how active the timer is

TimerTop::TimerTop(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent),
    ui(new Ui::TimerTop),
    m_updateTimer(new QTimer(this))
{
  Q_ASSERT(probe);

  ui->setupUi(this);
  ObjectTypeFilterProxyModel<QTimer> * const filterModel =
      new ObjectTypeFilterProxyModel<QTimer>(this);
  filterModel->setDynamicSortFilter(true);
  filterModel->setSourceModel(probe->objectListModel());
  TimerModel::instance()->setParent(this); // otherwise it's not filtered out
  TimerModel::instance()->setProbe(probe);
  TimerModel::instance()->setSourceModel(filterModel);
  QSortFilterProxyModel * const sortModel = new QSortFilterProxyModel(this);
  sortModel->setSourceModel(TimerModel::instance());
  sortModel->setDynamicSortFilter(true);
  ui->timerView->setModel(sortModel);
  ui->timerView->sortByColumn(TimerModel::WakeupsPerSecRole - TimerModel::FirstRole - 1,
                              Qt::DescendingOrder);

  m_updateTimer->setObjectName("GammaRay update timer");
  m_updateTimer->setSingleShot(false);
  m_updateTimer->setInterval(500);
  m_updateTimer->start();
  connect(m_updateTimer, SIGNAL(timeout()), this, SLOT(slotUpdateView()));
}

void TimerTop::slotUpdateView()
{
  ui->timerView->viewport()->update();
}

QStringList TimerTopFactory::supportedTypes() const
{
  return QStringList() << "QObject" << "QTimer";
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(TimerTopFactory)
#endif

#include "timertop.moc"
