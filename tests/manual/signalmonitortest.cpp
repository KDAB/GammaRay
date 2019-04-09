/*
  signalmonitortest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "signalmonitortest.h"

#include <QCoreApplication>
#include <QTimer>

SignalMonitorTest::SignalMonitorTest(QObject *parent)
    : QObject(parent)
    , m_timerCount(0)
{
    auto *t1 = new QTimer(this);
    t1->setObjectName(nextTimerName());
    t1->start(250);

    auto *t2 = new QTimer(this);
    t2->setObjectName(nextTimerName());
    connect(t2, &QTimer::timeout, this, &SignalMonitorTest::onTimeout);
    t2->start(1500);
}

QString SignalMonitorTest::nextTimerName()
{
    return QStringLiteral("SignalMonitorTest_t%1").arg(++m_timerCount);
}

void SignalMonitorTest::onTimeout()
{
    auto *tx = new QTimer(this);
    tx->setObjectName(nextTimerName());
    connect(tx, &QTimer::timeout, tx, &QObject::deleteLater);
    tx->start(2500);
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    new SignalMonitorTest(&app);
    return app.exec();
}
