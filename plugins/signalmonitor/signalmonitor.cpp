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

    m_historyModel = new SignalHistoryModel(probe, this);
    auto proxy = new ServerProxyModel<QSortFilterProxyModel>(this);
    proxy->setSourceModel(m_historyModel);
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
    // the model knows the row of the object, searching all of them takes long
    const auto sourceIndex = m_historyModel->indexForObject(obj);
    if (!sourceIndex.isValid())
        return;
    // the proxy only uses the model while it is used itself
    Model::used(m_objModel);
    const auto index = m_objModel->mapFromSource(sourceIndex);
    if (!index.isValid())
        return;

    m_objSelectionModel->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}
