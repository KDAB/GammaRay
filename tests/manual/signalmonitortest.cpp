/*
  signalmonitortest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
