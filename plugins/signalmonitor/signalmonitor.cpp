/*
  signalmonitor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

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

#include <QTimer>

using namespace GammaRay;

SignalMonitor::SignalMonitor(ProbeInterface *probe, QObject *parent)
  : SignalMonitorInterface(parent)
{
  StreamOperators::registerSignalMonitorStreamOperators();

  SignalHistoryModel *model = new SignalHistoryModel(probe, this);
  probe->registerModel("com.kdab.GammaRay.SignalHistoryModel", model);

  m_clock = new QTimer(this);
  m_clock->setInterval(1000/25); // update frequency of the delegate, we could slow this down a lot, and let the client interpolate, if necessary
  m_clock->setSingleShot(false);
  connect(m_clock, SIGNAL(timeout()), this, SLOT(timeout()));
}

SignalMonitor::~SignalMonitor()
{
}

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

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(SignalMonitorFactory)
#endif
