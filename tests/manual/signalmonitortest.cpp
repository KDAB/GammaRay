/*
  signalmonitortest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "signalmonitortest.h"

#include <QApplication>
#include <QTimer>

SignalMonitorTest::SignalMonitorTest(QObject *parent)
  : QObject(parent)
  , m_timerCount(0)
{
  QTimer *t1 = new QTimer(this);
  t1->setObjectName(nextTimerName());
  t1->start(250);

  QTimer *t2 = new QTimer(this);
  t2->setObjectName(nextTimerName());
  connect(t2, SIGNAL(timeout()), this, SLOT(onTimeout()));
  t2->start(1500);
}

QString SignalMonitorTest::nextTimerName()
{
  return QString::fromLatin1("SignalMonitorTest_t%1").arg(++m_timerCount);
}

void SignalMonitorTest::onTimeout()
{
  QTimer *tx = new QTimer(this);
  tx->setObjectName(nextTimerName());
  connect(tx, SIGNAL(timeout()), tx, SLOT(deleteLater()));
  tx->start(2500);
}

int main(int argc, char **argv)
{
  QApplication app(argc, argv);
  new SignalMonitorTest(&app);
  return app.exec();
}
