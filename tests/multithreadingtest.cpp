/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <probe/probecreator.h>
#include <probe/hooks.h>
#include <core/probe.h>

#include <QDebug>
#include <QtTest/qtest.h>
#include <QObject>
#include <QThread>
#include <QSignalSpy>

using namespace GammaRay;

class Thread : public QThread
{
    Q_OBJECT
public:
    Thread()
        : batchSize(1)
        , delay(0)
        , iterations(100) {}

    void run() Q_DECL_OVERRIDE
    {
        objects.reserve(batchSize);
        for (int i = 0; i < iterations; ++i) {
            for (int j = 0; j < batchSize; ++j)
                objects.push_back(new QObject);
            QTest::qWait(delay);
            qDeleteAll(objects);
            objects.clear();
        }
    }

    QVector<QObject *> objects;
    int batchSize;
    int delay;
    int iterations;
};

class MultiThreadingTest : public QObject
{
    Q_OBJECT
private:
    void createProbe()
    {
        qputenv("GAMMARAY_ProbePath", QCoreApplication::applicationDirPath().toUtf8());
        Hooks::installHooks();
        Probe::startupHookReceived();
        new ProbeCreator(ProbeCreator::Create);
        QTest::qWait(1); // event loop re-entry
    }

private slots:
    void testCreateDestroy_data()
    {
        QTest::addColumn<int>("batchSize");
        QTest::addColumn<int>("delay");
        QTest::addColumn<int>("iterations");

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
