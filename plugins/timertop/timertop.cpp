/*
  timertop.cpp

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

#include "timertop.h"
#include "timermodel.h"

#include <core/objecttypefilterproxymodel.h>
#include <core/signalspycallbackset.h>

#include <common/objectbroker.h>
#include <common/objectid.h>

#include <QCoreApplication>
#include <QItemSelectionModel>
#include <QtPlugin>
#include <QThread>

using namespace GammaRay;

//
// TODO list for timer plugin
//

// BUG: QTimeLine class name not shown
// Related: Protect against timer deletion
// BUG: Only top-level timers are shown (bug in probe)

// ! Wakeup time for QTimers
// ! Add button to view object info
// ! Test timer added/removed at runtime
// Buttons to kill or slow down timer and start timer
// Add big fat "total wakeups / sec" label to the status bar
// Retrieve receiver name from connection model
// Add to view as column: receivers: slotXYZ and 3 others (shown in tooltip)
// Move signal hook to probe interface
// Then maybe add general signal profiler plugin, or even visualization
// Flash delegate when timer triggered
// Color cell in view redish, depending on how active the timer is

class TimerFilterModel : public ObjectTypeFilterProxyModel<QTimer>
{
public:
    explicit TimerFilterModel(QObject *parent)
        : ObjectTypeFilterProxyModel<QTimer>(parent) {}

    bool filterAcceptsObject(QObject *object) const override
    {
        if (object && object->inherits("QQmlTimer"))
            return true;
        return ObjectTypeFilterProxyModel<QTimer>::filterAcceptsObject(object);
    }
};

static bool processCallback()
{
    return TimerModel::isInitialized();
}

static void signal_begin_callback(QObject *caller, int method_index, void **argv)
{
    Q_UNUSED(argv);
    if (!processCallback())
        return;
    TimerModel::instance()->preSignalActivate(caller, method_index);
}

static void signal_end_callback(QObject *caller, int method_index)
{
    // NOTE: here and below the caller may be invalid, e.g. if it was deleted from a slot
    if (!processCallback())
        return;
    TimerModel::instance()->postSignalActivate(caller, method_index);
}

TimerTop::TimerTop(Probe *probe, QObject *parent)
    : TimerTopInterface(parent)
{
    Q_ASSERT(probe);

    QSortFilterProxyModel * const filterModel = new TimerFilterModel(this);
    filterModel->setDynamicSortFilter(true);
    filterModel->setSourceModel(probe->objectListModel());
    TimerModel::instance()->setParent(this); // otherwise it's not filtered out
    TimerModel::instance()->setSourceModel(filterModel);

    SignalSpyCallbackSet callbacks;
    callbacks.signalBeginCallback = signal_begin_callback;
    callbacks.signalEndCallback = signal_end_callback;
    probe->registerSignalSpyCallbackSet(callbacks);

    probe->registerModel(QStringLiteral("com.kdab.GammaRay.TimerModel"), TimerModel::instance());
    m_selectionModel = ObjectBroker::selectionModel(TimerModel::instance());

    connect(probe, &Probe::objectSelected, this, &TimerTop::objectSelected);
}

void TimerTop::clearHistory()
{
    TimerModel::instance()->clearHistory();
}

void TimerTop::objectSelected(QObject* obj)
{
    auto timer = qobject_cast<QTimer*>(obj);
    if (!timer)
        return;

    const auto model = m_selectionModel->model();
    const auto indexList = model->match(model->index(0, 0), ObjectModel::ObjectIdRole,
                       QVariant::fromValue(ObjectId(timer)), 1,
                       Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
    if (indexList.isEmpty())
        return;

    const auto index = indexList.first();
    m_selectionModel->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

TimerTopFactory::TimerTopFactory(QObject *parent)
    : QObject(parent)
{
}
