/*
  timertoptest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "baseprobetest.h"
#include "testhelpers.h"

#include <plugins/timertop/timermodel.h>

#include <common/objectbroker.h>
#include <common/objectid.h>

#include <QAbstractItemModelTester>
#include <QSignalSpy>
#include <QTimer>

using namespace GammaRay;
using namespace TestHelpers;

class Deleter : public QObject
{
    Q_OBJECT
public:
    explicit Deleter(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

public slots:
    void deleteSender()
    {
        delete sender();
    }
};

class TimerTopTest : public BaseProbeTest
{
    Q_OBJECT
private:
    void createProbe() override
    {
        BaseProbeTest::createProbe();

        auto t = new QTimer; // trigger timer plugin activation
        QTest::qWait(1);
        delete t;
        QTest::qWait(1);
    }

private slots:
    void testTimerCreateDestroy()
    {
        createProbe();

        auto *model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TimerModel"));
        QVERIFY(model);
        QAbstractItemModelTester modelTest(model);
        auto baseRowCount = model->rowCount();
        QVERIFY(baseRowCount >= 0);
        QVERIFY(!searchFixedIndex(model, "timer1").isValid());

        auto t1 = new QTimer;
        t1->setObjectName("timer1");
        QTest::qWait(1);

        QCOMPARE(model->rowCount(), baseRowCount + 1);
        auto idx = searchFixedIndex(model, "QTimer (timer1)");
        QVERIFY(idx.isValid());
        QCOMPARE(idx.data(ObjectModel::ObjectIdRole).value<ObjectId>(), ObjectId(t1));

        delete t1;
        QTest::qWait(1);
        QCOMPARE(model->rowCount(), baseRowCount);
    }

    void testTimerActivation()
    {
        createProbe();

        auto *model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TimerModel"));
        QVERIFY(model);
        QAbstractItemModelTester modelTest(model);

        auto t1 = new QTimer;
        t1->setObjectName("timer1");
        t1->setInterval(10);
        t1->setSingleShot(true);
        QTest::qWait(1);

        auto idx = searchFixedIndex(model, "QTimer (timer1)");
        QVERIFY(idx.isValid());
        // TODO verify data

        QSignalSpy dataChangeSpy(model, &QAbstractItemModel::dataChanged);
        QVERIFY(dataChangeSpy.isValid());
        t1->start();

        // TODO verify data
        QVERIFY(!dataChangeSpy.isEmpty() || dataChangeSpy.wait(10 * 1000));
        QVERIFY(dataChangeSpy.size() < 5);

        delete t1;
        QTest::qWait(1);
    }

    void testTimerEvent()
    {
        createProbe();

        auto *model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TimerModel"));
        QVERIFY(model);
        auto idx = searchFixedIndex(model, "TimerTopTest (testObject)");
        QVERIFY(!idx.isValid());
        const auto prevRowCount = model->rowCount();

        setObjectName("testObject");
        auto timerId = startTimer(10);

        // The TimerModel does batch all by a 5000ms timer.
        // Wait for the free timer discovery
        QTRY_VERIFY_WITH_TIMEOUT(model->rowCount() != prevRowCount, 6000);

        idx = searchFixedIndex(model, "TimerTopTest (testObject)");
        QVERIFY(idx.isValid());
        QCOMPARE(idx.data(ObjectModel::ObjectIdRole).value<ObjectId>(), ObjectId(this));
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

            connect(mainThread.data(), &QThread::started, threadTimer.data(), static_cast<void (QTimer::*)()>(&QTimer::start));

            int timerId = -1;
            QTimer::singleShot(500, threadTimer.data(), [&]() {
                // "threadTimer" thread free timer operating on "threadTimer" object
                timerId = threadTimer->QObject::startTimer(250);
            });

            QSharedPointer<Deleter> deleter(new Deleter);
            auto deleteTimer = new QTimer;
            deleteTimer->setObjectName("deleteTimer");
            deleteTimer->setInterval(1500);
            connect(deleteTimer, &QTimer::timeout, deleter.data(), &Deleter::deleteSender);
            deleteTimer->start();

            mainThread->start();
            QTest::qWait(6000);

            QModelIndex idx;
            QVERIFY(model->rowCount() >= 4);

            idx = searchFixedIndex(model, "QThread (mainThread)");
            QVERIFY(idx.isValid());
            QCOMPARE(idx.data(ObjectModel::ObjectIdRole).value<ObjectId>(), ObjectId(mainThread.data()));

            idx = searchFixedIndex(model, "QTimer (mainTimer)");
            QVERIFY(idx.isValid());
            QCOMPARE(idx.data(ObjectModel::ObjectIdRole).value<ObjectId>(), ObjectId(mainTimer.data()));

            const auto idxs = searchFixedIndexes(model, "QTimer (threadTimer)");
            QCOMPARE(idxs.size(), 2);
            for (const QModelIndex &idx : idxs) {
                QVERIFY(idx.isValid());
            }

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
