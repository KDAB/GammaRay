/*
  connectionstest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <QCoreApplication>
#include <QThread>

class MyTestObject : public QObject
{
    Q_OBJECT
public slots:
    void mySlot1()
    {
    }
    void mySlot2()
    {
    }
    void mySlot3()
    {
    }
    void mySlot4()
    {
    }

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

static void dummyFunction()
{
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

    MyTestObject sender, receiver;
    sender.setObjectName(QStringLiteral("sender"));
    receiver.setObjectName(QStringLiteral("receiver"));
    connectObjects(&sender, &receiver);

    MyTestObject selfConnect;
    selfConnect.setObjectName(QStringLiteral("selfConnect"));
    connectObjects(&selfConnect, &selfConnect);

    QThread thread;
    thread.setObjectName(QStringLiteral("thread"));
    thread.start();

    MyTestObject threadSender, threadReceiver;
    threadSender.setObjectName(QStringLiteral("threadSender"));
    threadReceiver.setObjectName(QStringLiteral("threadReceiver"));
    threadSender.moveToThread(&thread);
    threadReceiver.moveToThread(&thread);

    MyTestObject localSender, localReceiver;
    localSender.setObjectName(QStringLiteral("localSender"));
    localReceiver.setObjectName(QStringLiteral("localReceiver"));

    connectObjects(&localSender, &threadReceiver);
    connectObjects(&threadSender, &localReceiver);

    MyTestObject doubleSender, doubleReceiver;
    doubleSender.setObjectName(QStringLiteral("doubleSender"));
    doubleReceiver.setObjectName(QStringLiteral("doubleReceiver"));
    connectObjects(&doubleSender, &doubleReceiver);
    connectObjects(&doubleSender, &doubleReceiver);

    MyTestObject lambdaSender, lambdaContext;
    lambdaSender.setObjectName(QStringLiteral("lambdaSender"));
    lambdaContext.setObjectName(QStringLiteral("lambdaContext"));
    QObject::connect(&lambdaSender, &MyTestObject::mySignal1, &dummyFunction);
    QObject::connect(&lambdaSender, &MyTestObject::mySignal2, &lambdaContext, &dummyFunction);

    return app.exec();
}

#include "connectionstest.moc"
