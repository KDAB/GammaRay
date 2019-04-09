/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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
    void testCreateDestroy_data()
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
