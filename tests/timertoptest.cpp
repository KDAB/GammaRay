/*
  timertoptest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>

#include <plugins/timertop/timermodel.h>

#include <probe/hooks.h>
#include <probe/probecreator.h>
#include <core/probe.h>
#include <common/objectbroker.h>
#include <common/objectid.h>

#include <3rdparty/qt/modeltest.h>

#include <QtTest/qtest.h>
#include <QObject>
#include <QSignalSpy>
#include <QTimer>

using namespace GammaRay;

class Deleter : public QObject
{
    Q_OBJECT
public:
    explicit Deleter(QObject *parent = nullptr)
        : QObject(parent)
    { }

public slots:
    void deleteSender()
    { delete sender(); }
};

class TimerTopTest : public QObject
{
    Q_OBJECT
private:
    void createProbe()
    {
        qputenv("GAMMARAY_ServerAddress", GAMMARAY_DEFAULT_LOCAL_TCP_URL);
        Hooks::installHooks();
        Probe::startupHookReceived();
        new ProbeCreator(ProbeCreator::Create);
        QTest::qWait(1); // event loop re-entry

        auto t = new QTimer; // trigger timer plugin activation
        QTest::qWait(1);
        delete t;
        QTest::qWait(1);
    }

    QModelIndex indexForName(QAbstractItemModel *model, const QString &name)
    {
        const auto matchResult = model->match(model->index(0, 0), Qt::DisplayRole, name, 1, Qt::MatchExactly | Qt::MatchRecursive);
        if (matchResult.size() < 1)
            return QModelIndex();
        const auto idx = matchResult.at(0);
        Q_ASSERT(idx.isValid());
        return idx;
    }

    QModelIndexList indexesForName(QAbstractItemModel *model, const QString &name)
    {
        const auto matches = model->match(model->index(0, 0), Qt::DisplayRole, name, -1, Qt::MatchExactly | Qt::MatchRecursive);
        foreach (const QModelIndex &idx, matches)
            Q_ASSERT(idx.isValid());
        return matches;
    }

private slots:
    void testTimerCreateDestroy()
    {
        createProbe();

        auto *model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TimerModel"));
        QVERIFY(model);
        ModelTest modelTest(model);
        auto baseRowCount = model->rowCount();
        QVERIFY(baseRowCount >= 0);
        QVERIFY(!indexForName(model, "timer1").isValid());

        auto t1 = new QTimer;
        t1->setObjectName("timer1");
        QTest::qWait(1);

        QCOMPARE(model->rowCount(), baseRowCount + 1);
        auto idx = indexForName(model, "timer1");
        QVERIFY(idx.isValid());
        QCOMPARE(idx.data(TimerModel::ObjectIdRole).value<ObjectId>(), ObjectId(t1));

        delete t1;
        QTest::qWait(1);
        QCOMPARE(model->rowCount(), baseRowCount);
    }

    void testTimerActivation()
    {
        createProbe();

        auto *model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TimerModel"));
        QVERIFY(model);
        ModelTest modelTest(model);

        auto t1 = new QTimer;
        t1->setObjectName("timer1");
        t1->setInterval(10);
        t1->setSingleShot(true);
        QTest::qWait(1);

        auto idx = indexForName(model, "timer1");
        QVERIFY(idx.isValid());
        // TODO verify data

        QSignalSpy dataChangeSpy(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)));
        QVERIFY(dataChangeSpy.isValid());
        t1->start();
        QTest::qWait(10 * 1000); // there's a 5sec throttle on dataChanged

        // TODO verify data
        QVERIFY(dataChangeSpy.size() > 0);
        QVERIFY(dataChangeSpy.size() < 5);

        delete t1;
        QTest::qWait(1);
    }

    void testTimerEvent()
    {
        createProbe();

        auto *model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TimerModel"));
        QVERIFY(model);
        auto idx = indexForName(model, "testObject");
        QVERIFY(!idx.isValid());

        setObjectName("testObject");
        auto timerId = startTimer(10);

        // The TimerModel does batch all by a 5000ms timer.
        QTest::qWait(5000);

        // Wait for the free timer discovery
        int i = 0;
        while (model->rowCount() == 0 && i++ < 10)
            QTest::qWait(100);

        idx = indexForName(model, "testObject");
        QVERIFY(idx.isValid());
        QCOMPARE(idx.data(TimerModel::ObjectIdRole).value<ObjectId>(), ObjectId(this));
        idx = idx.sibling(idx.row(), 6);
        QVERIFY(idx.isValid());
        QCOMPARE(idx.data().toInt(), timerId);

        killTimer(timerId);
        // remove free timers from model
        QMetaObject::invokeMethod(model, "clearHistory");
    }

    void testTimerMultithreading()
    {
        createProbe();

        {
            auto *model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TimerModel"));
            QVERIFY(model);

            QSharedPointer<QThread> mainThread(new QThread);
            mainThread->setObjectName("mainThread");

            // main thread free timer operating on "mainThread" object
            mainThread->startTimer(125);

            // main thread qtimer
            QSharedPointer<QTimer> mainTimer(new QTimer);
            mainTimer->setObjectName("mainTimer");
            mainTimer->setInterval(250);
            mainTimer->start();

            // "mainThread" thread "threadTimer" qtimer
            QSharedPointer<QTimer> threadTimer(new QTimer);
            threadTimer->setObjectName("threadTimer");
            threadTimer->setInterval(100);
            threadTimer->moveToThread(mainThread.data());

            connect(mainThread.data(), SIGNAL(started()), threadTimer.data(), SLOT(start()));

            int timerId = -1;
            QTimer::singleShot(500, threadTimer.data(), [&]() {
                // "threadTimer" thread free timer operating on "threadTimer" object
                timerId = threadTimer->QObject::startTimer(250);
            });

            QSharedPointer<Deleter> deleter(new Deleter);
            QTimer *deleteTimer = new QTimer;
            deleteTimer->setObjectName("deleteTimer");
            deleteTimer->setInterval(1500);
            connect(deleteTimer, SIGNAL(timeout()), deleter.data(), SLOT(deleteSender()));
            deleteTimer->start();

            mainThread->start();
            QTest::qWait(6000);

            QModelIndex idx;
            QCOMPARE(model->rowCount(), 4);

            idx = indexForName(model, "mainThread");
            QVERIFY(idx.isValid());
            QCOMPARE(idx.data(TimerModel::ObjectIdRole).value<ObjectId>(), ObjectId(mainThread.data()));

            idx = indexForName(model, "mainTimer");
            QVERIFY(idx.isValid());
            QCOMPARE(idx.data(TimerModel::ObjectIdRole).value<ObjectId>(), ObjectId(mainTimer.data()));

            auto idxs = indexesForName(model, "threadTimer");
            QCOMPARE(idxs.count(), 2);
            foreach (const QModelIndex &idx, idxs)
                QVERIFY(idx.isValid());

            QTimer::singleShot(0, threadTimer.data(), [&]() {
                threadTimer->QObject::killTimer(timerId);
                threadTimer->stop();
                mainThread->quit();
            });

            QVERIFY(mainThread->wait());

            // remove free timers from model
            QMetaObject::invokeMethod(model, "clearHistory");
        }

        QTest::qWait(1);
    }
};

QTEST_MAIN(TimerTopTest)

#include "timertoptest.moc"
