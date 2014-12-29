/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "modeltest.h"

#include <core/remote/remotemodelserver.h>
#include <client/remotemodel.h>
#include <common/message.h>

#include <QBuffer>
#include <QDebug>
#include <QtTest/qtest.h>
#include <QObject>
#include <QStandardItemModel>

using namespace GammaRay;

static void fakeRegisterServer() {}

namespace GammaRay {
class FakeRemoteModelServer : public RemoteModelServer
{
    Q_OBJECT
public:
    explicit FakeRemoteModelServer(const QString& objectName, QObject* parent = 0) : RemoteModelServer(objectName, parent)
    {
        m_myAddress = 42;
    }

    static void setup()
    {
        FakeRemoteModelServer::s_registerServerCallback = &fakeRegisterServer;
    }

signals:
    void message(const GammaRay::Message &msg);

private:
    bool isConnected() const Q_DECL_OVERRIDE { return true; }
    void sendMessage(const Message& msg) const Q_DECL_OVERRIDE
    {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::ReadWrite);
        msg.write(&buffer);
        buffer.seek(0);
        emit const_cast<FakeRemoteModelServer*>(this)->message(Message::readMessage(&buffer));
    }
};

class FakeRemoteModel : public RemoteModel
{
    Q_OBJECT
public:
    explicit FakeRemoteModel(const QString& serverObject, QObject* parent = 0) : RemoteModel(serverObject, parent)
    {
        m_myAddress = 42;
    }

    static void setup()
    {
        FakeRemoteModel::s_registerClientCallback = &fakeRegisterServer;
    }

signals:
    void message(const GammaRay::Message &msg);

private:
    virtual void sendMessage(const Message& msg) const Q_DECL_OVERRIDE
    {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::ReadWrite);
        msg.write(&buffer);
        buffer.seek(0);
        emit const_cast<FakeRemoteModel*>(this)->message(Message::readMessage(&buffer));
    }
};
}

class RemoteModelTest : public QObject
{
    Q_OBJECT
private slots:
    void initTestCases()
    {
        FakeRemoteModelServer::setup();
        FakeRemoteModel::setup();
    }

    void testEmptyRemoteModel()
    {
        auto emptyModel = new QStandardItemModel(this);

        FakeRemoteModelServer server("com.kdab.GammaRay.UnitTest.EmptyModel", this);
        server.setModel(emptyModel);
        server.modelMonitored(true);

        FakeRemoteModel client("com.kdab.GammaRay.UnitTest.EmptyModel", this);
        connect(&server, SIGNAL(message(GammaRay::Message)), &client, SLOT(newMessage(GammaRay::Message)));
        connect(&client, SIGNAL(message(GammaRay::Message)), &server, SLOT(newRequest(GammaRay::Message)));

        ModelTest modelTest(&client);

        QCOMPARE(client.rowCount(), 0);
        QTest::qWait(10);
        QCOMPARE(client.rowCount(), 0);
        QCOMPARE(client.hasChildren(), false);
    }

    void testListRemoteModel()
    {
        auto listModel = new QStandardItemModel(this);
        listModel->appendRow(new QStandardItem("entry0"));
        listModel->appendRow(new QStandardItem("entry2"));
        listModel->appendRow(new QStandardItem("entry3"));
        listModel->appendRow(new QStandardItem("entry4"));

        FakeRemoteModelServer server("com.kdab.GammaRay.UnitTest.ListModel", this);
        server.setModel(listModel);
        server.modelMonitored(true);

        FakeRemoteModel client("com.kdab.GammaRay.UnitTest.ListModel", this);
        connect(&server, SIGNAL(message(GammaRay::Message)), &client, SLOT(newMessage(GammaRay::Message)));
        connect(&client, SIGNAL(message(GammaRay::Message)), &server, SLOT(newRequest(GammaRay::Message)));

        ModelTest modelTest(&client);
        QTest::qWait(10); // ModelTest is going to fetch stuff for us already

        QCOMPARE(client.rowCount(), 4);
        QCOMPARE(client.hasChildren(), true);

        auto index = client.index(1, 0);
        QCOMPARE(index.data().toString(), QString("entry2"));
        QCOMPARE(client.rowCount(index), 0);

        listModel->insertRow(1, new QStandardItem("entry1"));
        QCOMPARE(client.rowCount(), 5);
        index =client.index(1, 0);
        QCOMPARE(index.data().toString(), QString("entry1"));

        listModel->takeRow(3);
        QCOMPARE(client.rowCount(), 4);
    }
};

QTEST_MAIN(RemoteModelTest)

#include "remotemodeltest.moc"
