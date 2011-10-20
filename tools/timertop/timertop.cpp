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
#include <3rdparty/qt/qobject_p_copy.h>

#include <QDebug>
#include <QMetaMethod>

#include <iostream>

// thread saftey!
// timer events

using namespace GammaRay;
using namespace std;

QTimer *timer_from_callback(QObject *caller, int method_index)
{
  QTimer * const timer = dynamic_cast<QTimer*>(caller);
  if (timer) {
    QMetaMethod method = timer->metaObject()->method(method_index);
    if (method.signature() == QLatin1String("timeout()")) {
      return timer;
    }
  }
  return 0;
}

static void signal_begin_callback(QObject *caller, int method_index, void **argv)
{
  Q_UNUSED(argv);
  QTimer * const timer = timer_from_callback(caller, method_index);
  if (timer) {
    cout << "--> QTimer::timeout() " << (void*)caller << " " << caller->objectName().toAscii().data() << endl;
  }
}

static void signal_end_callback(QObject *caller, int method_index)
{
  QTimer * const timer = timer_from_callback(caller, method_index);
  if (timer) {
    cout << "<-- QTimer::timeout() " << (void*)caller << " " << caller->objectName().toAscii().data() << endl;
  }
}

TimerTop::TimerTop(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent),
    ui(new Ui::TimerTop)
{  
  Q_UNUSED(probe);
  ui->setupUi(this);

  QSignalSpyCallbackSet callbacks;
  callbacks.slot_begin_callback = 0;
  callbacks.slot_end_callback = 0;
  callbacks.signal_begin_callback = signal_begin_callback;
  callbacks.signal_end_callback = signal_end_callback;

  qt_register_signal_spy_callbacks(callbacks);
}

#include "timertop.moc"
