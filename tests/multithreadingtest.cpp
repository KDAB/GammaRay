/*
  multithreadingtest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "baseprobetest.h"

#include <QDebug>
#include <QThread>
#include <QSignalSpy>

using namespace GammaRay;

class Thread : public QThread
{
    Q_OBJECT
public:
    Thread() = default;

    void run() override
    {
        objects.reserve(batchSize);
        for (int i = 0; i < iterations; ++i) {
            for (int j = 0; j < batchSize; ++j) {
                objects.push_back(new QObject);
            }
            QTest::qWait(delay);
            qDeleteAll(objects);
            objects.clear();
        }
    }

    QVector<QObject *> objects;
    int batchSize = 1;
    int delay = 0;
    int iterations = 100;
};

class MultiThreadingTest : public BaseProbeTest
{
    Q_OBJECT
private slots:
    static void testCreateDestroy_data()
    {
        QTest::addColumn<int>("batchSize", nullptr);
        QTest::addColumn<int>("delay", nullptr);
        QTest::addColumn<int>("iterations", nullptr);

        QTest::newRow("10-0-1000") << 10 << 0 << 1000;
        QTest::newRow("100-1-100") << 100 << 1 << 100;
        QTest::newRow("1000-10-100") << 1000 << 10 << 100;
    }

    void testCreateDestroy()
    {
        QFETCH(int, batchSize);
        QFETCH(int, delay);
        QFETCH(int, iterations);

        createProbe();

        Thread t;
        t.batchSize = batchSize;
        t.delay = delay;
        t.iterations = iterations;
        QTest::qWait(1);

        QSignalSpy spy(&t, SIGNAL(finished()));
        QVERIFY(spy.isValid());

        // this mainly aims at not triggering any of the sanity checks in the object models or Probe
        t.start();
        QVERIFY(spy.wait(30000));
    }
};

QTEST_MAIN(MultiThreadingTest)

#include "multithreadingtest.moc"
