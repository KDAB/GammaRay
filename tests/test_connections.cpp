#include "test_connections.h"

#include <QtCore/QDebug>
#include <QtGui/QApplication>
#include <QtCore/QTimer>
#include <QtTest/QtTestGui>
#include <QtCore/QProcessEnvironment>

const int TIMEOUTINTERVAL = 10;
const int OBJECTS = 50;
const int TIMEOUTS = 100;

TestConnections::TestConnections(TestConnections::Type type, int timeOuts)
: m_type(type), m_timeOuts(timeOuts), m_numTimeout(0)
{
  QTimer* t = new QTimer(this);
  connect(t, SIGNAL(timeout()), SLOT(timeout()));
  t->start(TIMEOUTINTERVAL);
}

TestConnections::~TestConnections()
{
}

void TestConnections::timeout()
{
  if (m_numTimeout == m_timeOuts) {
    qDeleteAll(m_threads);
    m_threads.clear();
    qDeleteAll(m_objects);
    m_objects.clear();
    emit done();
    return;
  }
  m_numTimeout++;

  if (m_type == NoEventLoop) {
    // directly create and delete objects without eventloop in between
    QObject* obj = new QObject(this);
    connect(obj, SIGNAL(destroyed(QObject*)), this, SLOT(dummyConnection()));
    delete obj;
  } else if (m_type == Stack) {
    QObject obj;
    connect(&obj, SIGNAL(destroyed(QObject*)), this, SLOT(dummyConnection()));
    disconnect(&obj, SIGNAL(destroyed(QObject*)), this, SLOT(dummyConnection()));
  } else if (m_type == Threaded) {
    QObject* obj = new QObject(this);
    connect(obj, SIGNAL(destroyed(QObject*)), this, SLOT(dummyConnection()));
    TestThread* thread = new TestThread(obj, this);
    m_threads << thread;
    thread->start();
  } else {
    // delete last objects
    for(int i = 0; i < m_objects.count(); ++i) {
      QObject* obj = m_objects.at(i);
      switch(m_type) {
        case Delete:
          delete obj;
          break;
        case DeleteLater:
          obj->deleteLater();
          break;
      }
    }
    m_objects.clear();

    // create some new objects
    for(int i = 0; i < OBJECTS; ++i) {
      QObject* obj = new QObject(this);
      m_objects << obj;
      connect(obj, SIGNAL(destroyed(QObject*)), this, SLOT(dummyConnection()));
    }
  }
}

TestThread::TestThread(QObject *obj, QObject *parent)
: QThread(parent), m_obj(obj)
{
  
}

TestThread::~TestThread()
{

}

void TestThread::run()
{
  QObject stackObj;
  connect(&stackObj, SIGNAL(destroyed(QObject*)), this, SLOT(dummySlot()));

  QObject* heapObj = new QObject;
  connect(heapObj, SIGNAL(destroyed(QObject*)), this, SLOT(dummySlot()));
  delete heapObj;

  connect(m_obj, SIGNAL(destroyed(QObject*)), this, SLOT(dummySlot()));
  delete m_obj;
}


void TestMain::run_data()
{
  QTest::addColumn<int>("type");
  QTest::newRow("delete") << static_cast<int>(TestConnections::Delete);
  QTest::newRow("deleteLater") << static_cast<int>(TestConnections::DeleteLater);
  QTest::newRow("noEventLoop") << static_cast<int>(TestConnections::NoEventLoop);
  QTest::newRow("stack") << static_cast<int>(TestConnections::Stack);
  QTest::newRow("threaded") << static_cast<int>(TestConnections::Threaded);
}

void TestMain::run()
{
  QFETCH(int, type);

  bool manual = QProcessEnvironment::systemEnvironment().value("ENDOSCOPE_TEST_MANUAL").toInt();
  TestConnections* tester = new TestConnections(static_cast<TestConnections::Type>(type),
                                                manual ? -1 : TIMEOUTS);

  QEventLoop* loop = new QEventLoop;
  connect(tester, SIGNAL(done()), loop, SLOT(quit()));
  loop->exec();
  delete loop;

  delete tester;
}


QTEST_MAIN(TestMain);

#include "test_connections.moc"
