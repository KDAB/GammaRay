/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include <QCoreApplication>
#include <QThread>

class MyTestObject : public QObject
{
    Q_OBJECT
public slots:
    void mySlot1() {}
    void mySlot2() {}
    void mySlot3() {}
    void mySlot4() {}

signals:
    void mySignal1();
    void mySignal2();
    void mySignal3();
    void mySignal4();
};

static void connectObjects(QObject *sender, QObject *receiver)
{
    QObject::connect(sender, SIGNAL(mySignal1()), receiver, SLOT(mySlot1()));
    QObject::connect(sender, SIGNAL(mySignal2()), receiver, SLOT(mySlot2()), Qt::DirectConnection);
    QObject::connect(sender, SIGNAL(mySignal3()), receiver, SLOT(mySlot3()), Qt::QueuedConnection);
    QObject::connect(sender, SIGNAL(mySignal4()), receiver, SLOT(mySlot4()), Qt::BlockingQueuedConnection);
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
static void dummyFunction()
{
}
#endif

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    MyTestObject sender, receiver;
    sender.setObjectName("sender");
    receiver.setObjectName("receiver");
    connectObjects(&sender, &receiver);

    MyTestObject selfConnect;
    selfConnect.setObjectName("selfConnect");
    connectObjects(&selfConnect, &selfConnect);

    QThread thread;
    thread.setObjectName("thread");
    thread.start();

    MyTestObject threadSender, threadReceiver;
    threadSender.setObjectName("threadSender");
    threadReceiver.setObjectName("threadReceiver");
    threadSender.moveToThread(&thread);
    threadReceiver.moveToThread(&thread);

    MyTestObject localSender, localReceiver;
    localSender.setObjectName("localSender");
    localReceiver.setObjectName("localReceiver");

    connectObjects(&localSender, &threadReceiver);
    connectObjects(&threadSender, &localReceiver);

    MyTestObject doubleSender, doubleReceiver;
    doubleSender.setObjectName("doubleSender");
    doubleReceiver.setObjectName("doubleReceiver");
    connectObjects(&doubleSender, &doubleReceiver);
    connectObjects(&doubleSender, &doubleReceiver);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    MyTestObject lambdaSender, lambdaContext;
    lambdaSender.setObjectName("lambdaSender");
    lambdaContext.setObjectName("lambdaContext");
    QObject::connect(&lambdaSender, &MyTestObject::mySignal1, &dummyFunction);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    QObject::connect(&lambdaSender, &MyTestObject::mySignal2, &lambdaContext, &dummyFunction);
#endif

    return app.exec();
}

#include "connectionstest.moc"

