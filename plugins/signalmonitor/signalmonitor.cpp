/*
  signalmonitor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

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

#include "signalmonitor.h"
#include "signalhistorymodel.h"
#include "relativeclock.h"
#include "signalmonitorcommon.h"

#include <core/remote/serverproxymodel.h>

#include <common/objectbroker.h>
#include <common/objectid.h>

#include <QItemSelectionModel>
#include <QTimer>

using namespace GammaRay;

SignalMonitor::SignalMonitor(Probe *probe, QObject *parent)
    : SignalMonitorInterface(parent)
{
    StreamOperators::registerSignalMonitorStreamOperators();

    auto *model = new SignalHistoryModel(probe, this);
    auto proxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    proxy->setDynamicSortFilter(true);
    proxy->setSourceModel(model);
    m_objModel = proxy;
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.SignalHistoryModel"), proxy);
    m_objSelectionModel = ObjectBroker::selectionModel(proxy);

    m_clock = new QTimer(this);
    m_clock->setInterval(1000/25); // update frequency of the delegate, we could slow this down a lot, and let the client interpolate, if necessary
    m_clock->setSingleShot(false);
    connect(m_clock, &QTimer::timeout, this, &SignalMonitor::timeout);

    connect(probe, &Probe::objectSelected, this, &SignalMonitor::objectSelected);
}

SignalMonitor::~SignalMonitor() = default;

void SignalMonitor::timeout()
{
    emit clock(RelativeClock::sinceAppStart()->mSecs());
}

void SignalMonitor::sendClockUpdates(bool enabled)
{
    if (enabled)
        m_clock->start();
    else
        m_clock->stop();
}

void SignalMonitor::objectSelected(QObject* obj)
{
    const auto indexList = m_objModel->match(m_objModel->index(0, 0), ObjectModel::ObjectIdRole,
        QVariant::fromValue<ObjectId>(ObjectId(obj)), 1, Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
    if (indexList.isEmpty())
        return;

    const auto index = indexList.first();
    m_objSelectionModel->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}
