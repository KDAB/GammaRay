/*
  test_connections.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_TEST_CONNECTIONS_H
#define GAMMARAY_TEST_CONNECTIONS_H

#include <QObject>
#include <QThread>

class QTimer;
class QEventLoop;

class TestObject : public QObject
{
  Q_OBJECT
  public:
    explicit TestObject(QObject *parent = 0);
    virtual ~TestObject();

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
    enum Type {
      DeleteLater,
      Delete,
      NoEventLoop,
      Stack,
      SetParent
    };

    TestConnections(Type type, int timeOuts, int timeoutInterval = -1);
    virtual ~TestConnections();

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
    QList<QObject*> m_objects;
    QTimer *m_timer;
};

class TestThread : public QThread
{
  Q_OBJECT
  public:
    TestThread(TestConnections::Type type, int timeOuts, int timeoutInterval = -1,
               QObject *parent = 0);
    virtual ~TestThread();
    virtual void run();

  private:
    TestConnections::Type m_type;
    int m_timeOuts;
    int m_timeoutInterval;
};

class TestMain : public QObject
{
  Q_OBJECT
  public:
    TestMain(int argc, char **m_argv);
  public slots:
    void startTests();
  private slots:
    void run_data();
    void run();
    void threading();
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
    QList<TestThread*> m_threads;
    QList<TestConnections*> m_tester;
    QEventLoop *m_loop;
};

#endif // TEST_CONNECTIONS_H
