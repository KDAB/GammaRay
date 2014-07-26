/*
  signalmonitor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QTimer>

using namespace GammaRay;

SignalMonitor::SignalMonitor(ProbeInterface *probe, QObject *parent)
  : SignalMonitorInterface(parent)
{
  SignalHistoryModel *model = new SignalHistoryModel(probe, this);
  probe->registerModel("com.kdab.GammaRay.SignalHistoryModel", model);

  QTimer *clock = new QTimer(this);
  clock->setInterval(1000/25); // update frequency of the delegate, we could slow this down a lot, and let the client interpolate, if necessary
  clock->setSingleShot(false);
  connect(clock, SIGNAL(timeout()), this, SLOT(timeout()));
  clock->start();
}

SignalMonitor::~SignalMonitor()
{
}

void SignalMonitor::timeout()
{
  emit clock(RelativeClock::sinceAppStart()->mSecs());
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(SignalMonitorFactory)
#endif
