/*
  test_connections.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "test_connections.h"

#include <compat/qasconst.h>

#include <QApplication>
#include <QDebug>
#include <QProcessEnvironment>
#include <QTimer>
#include <QtTestGui>

const int TIMEOUTINTERVAL = 10;
const int OBJECTS = 50;
const int TIMEOUTS = 100;

// BEGIN TestObject
TestObject::TestObject(QObject *parent)
    : QObject(parent)
// test object creation in ctor
    , child(new QObject(this))
{
    setObjectName(QStringLiteral("TestObject"));
    child->setObjectName(QStringLiteral("TestObjectChild"));
    // test connect/disconnect in ctor
    connect(child, &QObject::destroyed, this, &TestObject::dummySlot);
    disconnect(child, &QObject::destroyed, this, &TestObject::dummySlot);
    // now connect again for dtor
    connect(child, &QObject::destroyed, this, &TestObject::dummySlot);
}

TestObject::~TestObject()
{
    // test disconnect
    disconnect(child, &QObject::destroyed, this, &TestObject::dummySlot);
    // test connect, and leave it around to test disconnect-on-delete
    connect(child, &QObject::destroyed, this, &TestObject::dummySlot);
}

// END TestObject

// BEGIN TestConnections
TestConnections::TestConnections(TestConnections::Type type, int timeOuts, int timeoutInterval)
    : m_type(type)
    , m_timeOuts(timeOuts)
    , m_numTimeout(0)
    , m_timer(new QTimer(this))
{
    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &TestConnections::timeout);
    m_timer->start(timeoutInterval == -1 ? TIMEOUTINTERVAL : timeoutInterval);
}

TestConnections::~TestConnections() = default;

void TestConnections::timeout()
{
    if (m_numTimeout == m_timeOuts) {
        qDeleteAll(m_objects);
        m_objects.clear();
        emit done();
        delete m_timer;
        m_timer = nullptr;
        return;
    }
    m_numTimeout++;

    if (m_type == NoEventLoop) {
        // directly create and delete objects without eventloop in between
        QObject *obj = new TestObject(this);
        connect(obj, &QObject::destroyed, this, &TestConnections::dummySlot);
        delete obj;
    } else if (m_type == Stack) {
        QObject obj;
        connect(&obj, &QObject::destroyed, this, &TestConnections::dummySlot);
        disconnect(&obj, &QObject::destroyed, this, &TestConnections::dummySlot);
    } else if (m_type == SetParent) {
        auto *obj = new TestObject;
        obj->setParent(this);
        obj->child->setParent(nullptr);
        obj->child->setParent(obj);
        obj->deleteLater();
    } else {
        // delete last objects
        for (int i = 0; i < m_objects.count(); ++i) {
            QObject *obj = m_objects.at(i);
            switch (m_type) {
            case Delete:
                delete obj;
                break;
            case DeleteLater:
                obj->deleteLater();
                break;
            default:
                break;
            }
        }
        m_objects.clear();

        // create some new objects
        for (int i = 0; i < OBJECTS; ++i) {
            QObject *obj = new TestObject(this);
            m_objects << obj;
            connect(obj, &QObject::destroyed, this, &TestConnections::dummySlot);
        }
    }
}

// END TestConnections

// BEGIN TestThread
TestThread::TestThread(TestConnections::Type type, int timeOuts, int timeoutInterval,
                       QObject *parent)
    : QThread(parent)
    , m_type(type)
    , m_timeOuts(timeOuts)
    , m_timeoutInterval(timeoutInterval)
{
}

TestThread::~TestThread() = default;

void TestThread::run()
{
    TestConnections tester(m_type, m_timeOuts,
                           m_timeoutInterval == -1 ? TIMEOUTS : m_timeoutInterval);

    auto *loop = new QEventLoop;
    connect(&tester, &TestConnections::done, loop, &QEventLoop::quit);
    loop->exec();
    delete loop;
}

// END TestThread

// BEGIN TestWaiter
void TestWaiter::addTester(TestConnections *tester)
{
    connect(tester, &TestConnections::done, this, &TestWaiter::testerDone);
    m_tester << tester;
}

void TestWaiter::testerDone()
{
    TestConnections *tester = qobject_cast<TestConnections *>(sender());
    QVERIFY(tester);
    QVERIFY(m_tester.removeOne(tester));
    checkFinished();
}

void TestWaiter::addThread(TestThread *thread)
{
    connect(thread, &QThread::finished, this, &TestWaiter::threadFinished);
    m_threads << thread;
}

void TestWaiter::threadFinished()
{
    TestThread *thread = qobject_cast<TestThread *>(sender());
    QVERIFY(thread);
    QVERIFY(m_threads.removeOne(thread));
    checkFinished();
}

void TestWaiter::checkFinished()
{
    if (!m_loop)
        return;

    if (m_threads.isEmpty() && m_tester.isEmpty())
        m_loop->quit();
}

void TestWaiter::startThreadsAndWaitForFinished()
{
    if (m_threads.isEmpty() && m_tester.isEmpty())
        return;

    for (TestThread *thread : qAsConst(m_threads)) {
        thread->start();
    }

    m_loop = new QEventLoop;
    m_loop->exec();
    delete m_loop;
    m_loop = nullptr;
}

// END TestWaiter

// BEGIN TestMain

TestMain::TestMain(int argc, char **argv)
    : m_argc(argc)
    , m_argv(argv)
{
    QMetaObject::invokeMethod(this, "startTests", Qt::QueuedConnection);
}

void TestMain::startTests()
{
    qApp->exit(QTest::qExec(this, m_argc, m_argv));
}

void TestMain::run_data()
{
    QTest::addColumn<int>("type", nullptr);
    QTest::newRow("delete") << static_cast<int>(TestConnections::Delete);
    QTest::newRow("deleteLater") << static_cast<int>(TestConnections::DeleteLater);
    QTest::newRow("noEventLoop") << static_cast<int>(TestConnections::NoEventLoop);
    QTest::newRow("stack") << static_cast<int>(TestConnections::Stack);
    QTest::newRow("setParent") << static_cast<int>(TestConnections::SetParent);
}

void TestMain::run()
{
    QFETCH(int, type);

    bool manual
        = QProcessEnvironment::systemEnvironment().value(QStringLiteral("GAMMARAY_TEST_MANUAL")).
          toInt();
    TestConnections tester(static_cast<TestConnections::Type>(type),
                           manual ? -1 : TIMEOUTS);

    TestWaiter waiter;
    waiter.addTester(&tester);
    waiter.startThreadsAndWaitForFinished();
}

void TestMain::threading()
{
    TestWaiter waiter;
    const int timeouts = 10;
    // some testers to be run in the main thread
    // with varying timouts
    TestConnections tester1(TestConnections::NoEventLoop, timeouts, 10);
    waiter.addTester(&tester1);
    TestConnections tester2(TestConnections::Delete, timeouts, 11);
    waiter.addTester(&tester2);
    TestConnections tester3(TestConnections::DeleteLater, timeouts, 12);
    waiter.addTester(&tester3);
    TestConnections tester4(TestConnections::Stack, timeouts, 13);
    waiter.addTester(&tester4);
    TestConnections tester5(TestConnections::SetParent, timeouts, 14);
    waiter.addTester(&tester5);
    // now some threads
    TestThread thread1(TestConnections::NoEventLoop, timeouts, 10);
    waiter.addThread(&thread1);
    TestThread thread2(TestConnections::Delete, timeouts, 11);
    waiter.addThread(&thread2);
    TestThread thread3(TestConnections::DeleteLater, timeouts, 12);
    waiter.addThread(&thread3);
    TestThread thread4(TestConnections::Stack, timeouts, 13);
    waiter.addThread(&thread4);
    TestThread thread5(TestConnections::SetParent, timeouts, 13);
    waiter.addThread(&thread5);

    waiter.startThreadsAndWaitForFinished();
}

// END TestMain

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMain tc(argc, argv);
    return app.exec();
}
