/*
  test_connections.h

  This file is part of Endoscope, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef TEST_CONNECTIONS_H
#define TEST_CONNECTIONS_H

#include <QtCore/QObject>
#include <QtCore/QThread>

class TestConnections : public QObject {
  Q_OBJECT
public:
  enum Type {
    DeleteLater,
    Delete,
    NoEventLoop,
    Stack,
    Threaded
  };
  TestConnections(Type type, int timeOuts);
  virtual ~TestConnections();
public slots:
  void timeout();
  void dummyConnection(){}
signals:
  void done();
private:
  const Type m_type;
  const int m_timeOuts;
  int m_numTimeout;
  QList<QObject*> m_objects;
  QList<QThread*> m_threads;
};

class TestThread : public QThread {
  Q_OBJECT
public:
  TestThread(QObject* obj, QObject* parent);
  virtual ~TestThread();
  virtual void run();
  // prevent deadlock...
  bool isRunningNoLock() const;
public slots:
  void dummySlot(){}
private:
  QObject* m_obj;
};

class TestMain : public QObject {
  Q_OBJECT
private slots:
  void run_data();
  void run();
};

#endif // TEST_CONNECTIONS_H
