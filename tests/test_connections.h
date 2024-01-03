/*
  test_connections.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_TEST_CONNECTIONS_H
#define GAMMARAY_TEST_CONNECTIONS_H

#include <QObject>
#include <QThread>

QT_BEGIN_NAMESPACE
class QTimer;
class QEventLoop;
QT_END_NAMESPACE

class TestObject : public QObject
{
    Q_OBJECT
public:
    explicit TestObject(QObject *parent = nullptr);
    ~TestObject() override;

private slots:
    void dummySlot()
    {
    }

public:
    QObject *child;
};

class TestConnections : public QObject
{
    Q_OBJECT
public:
    enum Type
    {
        DeleteLater,
        Delete,
        NoEventLoop,
        Stack,
        SetParent
    };

    TestConnections(Type type, int timeOuts, int timeoutInterval = -1);
    ~TestConnections() override;

public slots:
    void timeout();

private slots:
    void dummySlot()
    {
    }

signals:
    void done();

private:
    const Type m_type;
    const int m_timeOuts;
    int m_numTimeout;
    QList<QObject *> m_objects;
    QTimer *m_timer;
};

class TestThread : public QThread
{
    Q_OBJECT
public:
    TestThread(TestConnections::Type type, int timeOuts, int timeoutInterval = -1,
               QObject *parent = nullptr);
    ~TestThread() override;
    void run() override;

private:
    TestConnections::Type m_type;
    int m_timeOuts;
    int m_timeoutInterval;
};

class TestMain : public QObject
{
    Q_OBJECT
public:
    TestMain(int argc, char **argv);
public slots:
    void startTests();
private slots:
    static void run_data();
    static void run();
    static void threading();

private:
    int m_argc;
    char **m_argv;
};

class TestWaiter : public QObject
{
    Q_OBJECT
public:
    void addThread(TestThread *thread);
    void addTester(TestConnections *tester);
    void startThreadsAndWaitForFinished();

private slots:
    void testerDone();
    void threadFinished();

private:
    void checkFinished();
    QList<TestThread *> m_threads;
    QList<TestConnections *> m_tester;
    QEventLoop *m_loop = nullptr;
};

#endif // TEST_CONNECTIONS_H
