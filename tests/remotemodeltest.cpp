/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include <3rdparty/qt/modeltest.h>

#include <core/remote/remotemodelserver.h>
#include <client/remotemodel.h>
#include <common/message.h>

#include <QBuffer>
#include <QDebug>
#include <QtTest/qtest.h>
#include <QObject>
#include <QSortFilterProxyModel>
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
    void sendMessage(const Message& msg) const Q_DECL_OVERRIDE
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

        FakeRemoteModelServer server(QStringLiteral("com.kdab.GammaRay.UnitTest.EmptyModel"), this);
        server.setModel(emptyModel);
        server.modelMonitored(true);

        FakeRemoteModel client(QStringLiteral("com.kdab.GammaRay.UnitTest.EmptyModel"), this);
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
        listModel->appendRow(new QStandardItem(QStringLiteral("entry0")));
        listModel->appendRow(new QStandardItem(QStringLiteral("entry2")));
        listModel->appendRow(new QStandardItem(QStringLiteral("entry3")));
        listModel->appendRow(new QStandardItem(QStringLiteral("entry4")));

        FakeRemoteModelServer server(QStringLiteral("com.kdab.GammaRay.UnitTest.ListModel"), this);
        server.setModel(listModel);
        server.modelMonitored(true);

        FakeRemoteModel client(QStringLiteral("com.kdab.GammaRay.UnitTest.ListModel"), this);
        connect(&server, SIGNAL(message(GammaRay::Message)), &client, SLOT(newMessage(GammaRay::Message)));
        connect(&client, SIGNAL(message(GammaRay::Message)), &server, SLOT(newRequest(GammaRay::Message)));

        ModelTest modelTest(&client);
        QTest::qWait(10); // ModelTest is going to fetch stuff for us already

        QCOMPARE(client.rowCount(), 4);
        QCOMPARE(client.hasChildren(), true);

        auto index = client.index(1, 0);
        index.data(); // need an event loop entry for the data retrieval
        QTest::qWait(1);
        QCOMPARE(index.data().toString(), QStringLiteral("entry2"));
        QCOMPARE(client.rowCount(index), 0);

        listModel->insertRow(1, new QStandardItem(QStringLiteral("entry1")));
        QCOMPARE(client.rowCount(), 5);
        index = client.index(1, 0);
        index.data(); // need an event loop entry for the data retrieval
        QTest::qWait(1);
        QCOMPARE(index.data().toString(), QStringLiteral("entry1"));

        listModel->takeRow(3);
        QCOMPARE(client.rowCount(), 4);
    }

    void testTreeRemoteModel()
    {
        auto treeModel = new QStandardItemModel(this);
        auto e0 = new QStandardItem(QStringLiteral("entry0"));
        e0->appendRow(new QStandardItem(QStringLiteral("entry00")));
        e0->appendRow(new QStandardItem(QStringLiteral("entry01")));
        treeModel->appendRow(e0);
        auto e1 = new QStandardItem(QStringLiteral("entry1"));
        e1->appendRow(new QStandardItem(QStringLiteral("entry10")));
        e1->appendRow(new QStandardItem(QStringLiteral("entry12")));
        treeModel->appendRow(e1);

        FakeRemoteModelServer server(QStringLiteral("com.kdab.GammaRay.UnitTest.TreeModel"), this);
        server.setModel(treeModel);
        server.modelMonitored(true);

        FakeRemoteModel client(QStringLiteral("com.kdab.GammaRay.UnitTest.TreeModel"), this);
        connect(&server, SIGNAL(message(GammaRay::Message)), &client, SLOT(newMessage(GammaRay::Message)));
        connect(&client, SIGNAL(message(GammaRay::Message)), &server, SLOT(newRequest(GammaRay::Message)));

        ModelTest modelTest(&client);
        QTest::qWait(10); // ModelTest is going to fetch stuff for us already

        QCOMPARE(client.rowCount(), 2);
        QCOMPARE(client.hasChildren(), true);

        auto i1 = client.index(1, 0);
        i1.data(); // need an event loop entry for the data retrieval
        QTest::qWait(1);
        QCOMPARE(i1.data().toString(), QStringLiteral("entry1"));
        QCOMPARE(client.rowCount(i1), 2);

        auto i12 = client.index(1, 0, i1);
        i12.data(); // need an event loop entry for the data retrieval
        QTest::qWait(1);
        QCOMPARE(i12.data().toString(), QStringLiteral("entry12"));
        QCOMPARE(client.rowCount(i12), 0);

        e1->insertRow(1, new QStandardItem(QStringLiteral("entry11")));
        QCOMPARE(client.rowCount(i1), 3);
        auto i11 = client.index(1, 0, i1);
        i11.data(); // need an event loop entry for the data retrieval
        QTest::qWait(1);
        QCOMPARE(i11.data().toString(), QStringLiteral("entry11"));
        QCOMPARE(client.rowCount(i11), 0);

        e1->takeRow(0);
        QCOMPARE(client.rowCount(i1), 2);
        i11 = client.index(0, 0, i1);
        QCOMPARE(i11.data().toString(), QStringLiteral("entry11"));
    }

    // this should not make a difference if the above works, however it broke massively with Qt 5.4...
    void testSortProxy()
    {
        auto treeModel = new QStandardItemModel(this);
        auto e0 = new QStandardItem(QStringLiteral("entry1"));
        e0->appendRow(new QStandardItem(QStringLiteral("entry10")));
        e0->appendRow(new QStandardItem(QStringLiteral("entry11")));
        treeModel->appendRow(e0);
        auto e1 = new QStandardItem(QStringLiteral("entry0"));
        e1->appendRow(new QStandardItem(QStringLiteral("entry00")));
        e1->appendRow(new QStandardItem(QStringLiteral("entry01")));
        e1->appendRow(new QStandardItem(QStringLiteral("entry02")));
        e1->appendRow(new QStandardItem(QStringLiteral("entry03")));
        treeModel->appendRow(e1);

        FakeRemoteModelServer server(QStringLiteral("com.kdab.GammaRay.UnitTest.TreeModel2"), this);
        server.setModel(treeModel);
        server.modelMonitored(true);

        FakeRemoteModel client(QStringLiteral("com.kdab.GammaRay.UnitTest.TreeModel2"), this);
        connect(&server, SIGNAL(message(GammaRay::Message)), &client, SLOT(newMessage(GammaRay::Message)));
        connect(&client, SIGNAL(message(GammaRay::Message)), &server, SLOT(newRequest(GammaRay::Message)));

        QSortFilterProxyModel proxy;
        proxy.setDynamicSortFilter(true);
        proxy.sort(0);
        proxy.setSourceModel(&client);

        ModelTest modelTest(&proxy);
        QTest::qWait(10); // ModelTest is going to fetch stuff for us already

        QCOMPARE(client.rowCount(), 2);
        QCOMPARE(proxy.rowCount(), 2);

        auto pi0 = proxy.index(0, 0);
        QCOMPARE(pi0.data().toString(), QStringLiteral("entry0"));
        QCOMPARE(proxy.rowCount(pi0), 4);

        auto pi03 = proxy.index(3, 0, pi0);
        QCOMPARE(pi03.data().toString(), QStringLiteral("entry03"));

        auto ci0 = client.index(0, 0);
        QCOMPARE(ci0.data().toString(), QStringLiteral("entry1"));
        QCOMPARE(client.rowCount(ci0), 2);

        auto pi1 = proxy.index(1, 0);
        QCOMPARE(pi1.data().toString(), QStringLiteral("entry1"));
        // this fails with data() call batching sizes close to 1
//         QEXPECT_FAIL("", "QSFPM misbehavior, no idea yet where this is coming from", Continue);
        QCOMPARE(proxy.rowCount(pi1), 2);
    }
};

QTEST_MAIN(RemoteModelTest)

#include "remotemodeltest.moc"
