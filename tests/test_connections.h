#ifndef TEST_CONNECTIONS_H
#define TEST_CONNECTIONS_H

#include <QtCore/QObject>

class TestConnections : public QObject {
  Q_OBJECT
public:
  enum Type {
    DeleteLater,
    Delete,
    NoEventLoop,
    Stack
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
};

class TestMain : public QObject {
  Q_OBJECT
private slots:
  void run_data();
  void run();
};

#endif // TEST_CONNECTIONS_H