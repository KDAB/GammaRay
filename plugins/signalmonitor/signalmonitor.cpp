/*
  signalmonitor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    proxy->setSourceModel(model);
    m_objModel = proxy;
    probe->registerModel(QStringLiteral("com.kdab.GammaRay.SignalHistoryModel"), proxy);
    m_objSelectionModel = ObjectBroker::selectionModel(proxy);

    m_clock = new QTimer(this);
    m_clock->setInterval(1000 / 25); // update frequency of the delegate, we could slow this down a lot, and let the client interpolate, if necessary
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

void SignalMonitor::objectSelected(QObject *obj)
{
    const auto indexList = m_objModel->match(m_objModel->index(0, 0), ObjectModel::ObjectIdRole,
                                             QVariant::fromValue<ObjectId>(ObjectId(obj)), 1, Qt::MatchExactly | Qt::MatchRecursive | Qt::MatchWrap);
    if (indexList.isEmpty())
        return;

    const auto &index = indexList.first();
    m_objSelectionModel->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}
