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