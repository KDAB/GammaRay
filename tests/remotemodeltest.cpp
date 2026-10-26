/*
  remotemodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/


#include <core/remote/remotemodelserver.h>
#include <client/remotemodel.h>
#include <common/message.h>

#include <QAbstractItemModelTester>
#include <QBuffer>
#include <QDebug>
#include <QObject>
#include <QSignalSpy>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QRegularExpression>
#include <QScopeGuard>
#include <QStringListModel>
#include <QTest>

using namespace GammaRay;

static void fakeRegisterServer()
{
}

namespace GammaRay {
class FakeRemoteModelServer : public RemoteModelServer
{
    Q_OBJECT
public:
    explicit FakeRemoteModelServer(const QString &objectName, QObject *parent = nullptr)
        : RemoteModelServer(objectName, parent)
    {
        m_myAddress = 42;
    }

    static void setup()
    {
        FakeRemoteModelServer::s_registerServerCallback = &fakeRegisterServer;
    }

signals:
    void message(const GammaRay::Message &);

private slots:
    void deliverMessage(const QByteArray &ba)
    {
        QBuffer buffer(const_cast<QByteArray *>(&ba));
        buffer.open(QIODevice::ReadOnly);
        emit message(Message::readMessage(&buffer));
    }

private:
    bool isConnected() const override
    {
        return true;
    }
    void sendMessage(const Message &msg) const override
    {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::WriteOnly);
        msg.write(&buffer);
        buffer.close();
        QMetaObject::invokeMethod(const_cast<FakeRemoteModelServer *>(this), "deliverMessage", Qt::QueuedConnection, Q_ARG(QByteArray, ba));
    }
};

class FakeRemoteModel : public RemoteModel
{
    Q_OBJECT
public:
    explicit FakeRemoteModel(const QString &serverObject, QObject *parent = nullptr)
        : RemoteModel(serverObject, parent)
    {
        m_myAddress = 42;
    }

    static void setup()
    {
        FakeRemoteModel::s_registerClientCallback = &fakeRegisterServer;
    }

    // the client asserts when it gets out of sync, for tests that break the source model on purpose
    static auto allowOutOfSync()
    {
        s_assertInSync = false;
        return qScopeGuard([] { s_assertInSync = true; });
    }

signals:
    void message(const GammaRay::Message &);

private:
    void sendMessage(const Message &msg) const override
    {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::ReadWrite);
        msg.write(&buffer);
        buffer.seek(0);
        emit const_cast<FakeRemoteModel *>(this)->message(Message::readMessage(&buffer));
    }
};
}

// changes its rows without telling anyone, like a broken source model would
class UnsignaledListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    using QAbstractListModel::QAbstractListModel;

    int rowCount(const QModelIndex &parent = {}) const override
    {
        return parent.isValid() ? 0 : m_rows.size();
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role != Qt::DisplayRole)
            return {};
        return m_rows.at(index.row());
    }

    void appendWithoutSignals(const QString &row)
    {
        m_rows.push_back(row);
    }

    void removeRowWithSignals(int row)
    {
        beginRemoveRows({}, row, row);
        m_rows.removeAt(row);
        endRemoveRows();
    }

private:
    QStringList m_rows;
};

class RemoteModelTest : public QObject
{
    Q_OBJECT
private:
    // fetches the data of all top-level rows
    static QStringList rowData(const QAbstractItemModel *model)
    {
        QStringList rows;
        for (int row = 0; row < model->rowCount(); ++row) {
            const auto index = model->index(row, 0);
            if (!waitForData(index))
                return {};
            rows.push_back(index.data().toString());
        }
        return rows;
    }
    static bool waitForData(const QModelIndex &idx)
    {
        if (idx.data(RemoteModelRole::LoadingState).value<RemoteModelNodeState::NodeStates>() == RemoteModelNodeState::NoState)
            return true; // data already present

        QSignalSpy spy(const_cast<QAbstractItemModel *>(idx.model()), &QAbstractItemModel::dataChanged);
        if (!spy.isValid())
            return false;
        idx.data(); // trigger the request
        Q_ASSERT(spy.isEmpty());
        while (spy.wait()) {
            for (auto it = spy.constBegin(); it != spy.constEnd(); ++it) {
                if ((*it).contains(idx))
                    return true;
            }
            spy.clear();
        }
        return false;
    }

private slots:
    static void initTestCase()
    {
        FakeRemoteModelServer::setup();
        FakeRemoteModel::setup();
    }

    void testEmptyRemoteModel()
    {
        QScopedPointer<QStandardItemModel> emptyModel(new QStandardItemModel(this));

        FakeRemoteModelServer server(QStringLiteral("com.kdab.GammaRay.UnitTest.EmptyModel"), this);
        server.setModel(emptyModel.data());
        server.modelMonitored(true);

        FakeRemoteModel client(QStringLiteral("com.kdab.GammaRay.UnitTest.EmptyModel"), this);
        connect(&server, &FakeRemoteModelServer::message, &client,
                &RemoteModel::newMessage);
        connect(&client, &FakeRemoteModel::message, &server,
                &RemoteModelServer::newRequest);

        QAbstractItemModelTester modelTest(&client);

        QCOMPARE(client.rowCount(), 0);
        QTest::qWait(10);
        QCOMPARE(client.rowCount(), 0);
        QCOMPARE(client.hasChildren(), false);
    }

    void testListRemoteModel()
    {
        QScopedPointer<QStandardItemModel> listModel(new QStandardItemModel(this));
        listModel->appendRow(new QStandardItem(QStringLiteral("entry0")));
        listModel->appendRow(new QStandardItem(QStringLiteral("entry2")));
        listModel->appendRow(new QStandardItem(QStringLiteral("entry3")));
        listModel->appendRow(new QStandardItem(QStringLiteral("entry4")));

        FakeRemoteModelServer server(QStringLiteral("com.kdab.GammaRay.UnitTest.ListModel"), this);
        server.setModel(listModel.data());
        server.modelMonitored(true);

        FakeRemoteModel client(QStringLiteral("com.kdab.GammaRay.UnitTest.ListModel"), this);
        connect(&server, &FakeRemoteModelServer::message, &client,
                &RemoteModel::newMessage);
        connect(&client, &FakeRemoteModel::message, &server,
                &RemoteModelServer::newRequest);

        QAbstractItemModelTester modelTest(&client);
        QTRY_VERIFY(client.rowCount() == 4); // ModelTest is going to fetch stuff for us already

        QCOMPARE(client.rowCount(), 4);
        QCOMPARE(client.hasChildren(), true);

        auto index = client.index(1, 0);
        QVERIFY(waitForData(index));
        QCOMPARE(index.data().toString(), QStringLiteral("entry2"));
        QCOMPARE(client.rowCount(index), 0);

        listModel->insertRow(1, new QStandardItem(QStringLiteral("entry1")));
        QTRY_VERIFY(client.rowCount() == 5);
        QCOMPARE(client.rowCount(), 5);
        index = client.index(1, 0);
        QVERIFY(waitForData(index));
        QCOMPARE(index.data().toString(), QStringLiteral("entry1"));

        const auto deleteMe = listModel->takeRow(3);
        qDeleteAll(deleteMe);
        QTRY_VERIFY(client.rowCount() == 4);
        QCOMPARE(client.rowCount(), 4);
    }

    void testTreeRemoteModel()
    {
        QScopedPointer<QStandardItemModel> treeModel(new QStandardItemModel(this));
        auto e0 = new QStandardItem(QStringLiteral("entry0"));
        e0->appendRow(new QStandardItem(QStringLiteral("entry00")));
        e0->appendRow(new QStandardItem(QStringLiteral("entry01")));
        treeModel->appendRow(e0);
        auto e1 = new QStandardItem(QStringLiteral("entry1"));
        e1->appendRow(new QStandardItem(QStringLiteral("entry10")));
        e1->appendRow(new QStandardItem(QStringLiteral("entry12")));
        treeModel->appendRow(e1);

        FakeRemoteModelServer server(QStringLiteral("com.kdab.GammaRay.UnitTest.TreeModel"), this);
        server.setModel(treeModel.data());
        server.modelMonitored(true);

        FakeRemoteModel client(QStringLiteral("com.kdab.GammaRay.UnitTest.TreeModel"), this);
        connect(&server, &FakeRemoteModelServer::message, &client,
                &RemoteModel::newMessage);
        connect(&client, &FakeRemoteModel::message, &server,
                &RemoteModelServer::newRequest);

        QAbstractItemModelTester modelTest(&client);
        QTRY_VERIFY(client.rowCount() == 2);

        QCOMPARE(client.rowCount(), 2);
        QCOMPARE(client.hasChildren(), true);

        auto i1 = client.index(1, 0);
        QVERIFY(waitForData(i1));
        QCOMPARE(i1.data().toString(), QStringLiteral("entry1"));
        QCOMPARE(client.rowCount(i1), 2);

        auto i12 = client.index(1, 0, i1);
        QVERIFY(waitForData(i12));
        QCOMPARE(i12.data().toString(), QStringLiteral("entry12"));
        QCOMPARE(client.rowCount(i12), 0);

        e1->insertRow(1, new QStandardItem(QStringLiteral("entry11")));
        QTRY_VERIFY(client.rowCount(i1) == 3);

        QCOMPARE(client.rowCount(i1), 3);
        auto i11 = client.index(1, 0, i1);
        QVERIFY(waitForData(i11));
        QCOMPARE(i11.data().toString(), QStringLiteral("entry11"));
        QCOMPARE(client.rowCount(i11), 0);

        const auto deleteMe = e1->takeRow(0);
        qDeleteAll(deleteMe);
        QTRY_VERIFY(client.rowCount(i1) == 2);

        QCOMPARE(client.rowCount(i1), 2);
        i11 = client.index(0, 0, i1);
        QVERIFY(waitForData(i11));
        QCOMPARE(i11.data().toString(), QStringLiteral("entry11"));
    }

    // this should not make a difference if the above works, however it broke massively with Qt 5.4...
    void testSortProxy()
    {
        QScopedPointer<QStandardItemModel> treeModel(new QStandardItemModel(this));
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
        server.setModel(treeModel.data());
        server.modelMonitored(true);

        FakeRemoteModel client(QStringLiteral("com.kdab.GammaRay.UnitTest.TreeModel2"), this);
        connect(&server, &FakeRemoteModelServer::message, &client,
                &RemoteModel::newMessage);
        connect(&client, &FakeRemoteModel::message, &server,
                &RemoteModelServer::newRequest);

        QSortFilterProxyModel proxy;
        proxy.setDynamicSortFilter(true);
        proxy.sort(0);
        proxy.setSourceModel(&client);

        QAbstractItemModelTester modelTest(&proxy);
        QTRY_VERIFY(client.rowCount() == 2);

        QCOMPARE(client.rowCount(), 2);
        QCOMPARE(proxy.rowCount(), 2);

        auto pi0 = proxy.index(0, 0);
        QVERIFY(waitForData(pi0));
        QCOMPARE(pi0.data().toString(), QStringLiteral("entry0"));
        QCOMPARE(proxy.rowCount(pi0), 4);

        auto pi03 = proxy.index(3, 0, pi0);
        QVERIFY(waitForData(pi03));
        QCOMPARE(pi03.data().toString(), QStringLiteral("entry03"));

        auto ci0 = client.index(0, 0);
        QVERIFY(waitForData(ci0));
        QCOMPARE(ci0.data().toString(), QStringLiteral("entry1"));
        QCOMPARE(client.rowCount(ci0), 2);

        auto pi1 = proxy.index(1, 0);
        QVERIFY(waitForData(pi1));
        QCOMPARE(pi1.data().toString(), QStringLiteral("entry1"));
        // this fails with data() call batching sizes close to 1
        // QEXPECT_FAIL("", "QSFPM misbehavior, no idea yet where this is coming from", Continue);
        QCOMPARE(proxy.rowCount(pi1), 2);
    }

    static void testMoveRows_data()
    {
        QTest::addColumn<int>("first");
        QTest::addColumn<int>("last");
        QTest::addColumn<int>("destination");

        QTest::newRow("one row up") << 3 << 3 << 1;
        QTest::newRow("one row down") << 1 << 1 << 4;
        QTest::newRow("several rows up") << 3 << 4 << 0;
        QTest::newRow("several rows up, overlapping") << 1 << 3 << 0;
        QTest::newRow("several rows down") << 0 << 1 << 5;
        QTest::newRow("to the end") << 0 << 0 << 6;
    }

    void testMoveRows()
    {
        QFETCH(int, first);
        QFETCH(int, last);
        QFETCH(int, destination);

        QStringListModel listModel({ "0", "1", "2", "3", "4", "5" });

        FakeRemoteModelServer server(QStringLiteral("com.kdab.GammaRay.UnitTest.MoveModel"), this);
        server.setModel(&listModel);
        server.modelMonitored(true);

        FakeRemoteModel client(QStringLiteral("com.kdab.GammaRay.UnitTest.MoveModel"), this);
        connect(&server, &FakeRemoteModelServer::message, &client, &RemoteModel::newMessage);
        connect(&client, &FakeRemoteModel::message, &server, &RemoteModelServer::newRequest);

        QAbstractItemModelTester modelTest(&client);
        QTRY_COMPARE(client.rowCount(), 6);
        QCOMPARE(rowData(&client), listModel.stringList());

        QVERIFY(listModel.moveRows({}, first, last - first + 1, {}, destination));
        QTRY_COMPARE(rowData(&client), listModel.stringList());
    }

    // rows inserted together with others shift the rows whose data was requested meanwhile, that
    // data has to arrive even when nobody asks for it again, like a proxy that filtered these rows
    // out as they only had placeholder data
    void testInsertRowsWithFilterProxy()
    {
        QStandardItemModel model;
        for (int i = 0; i < 10; ++i)
            model.appendRow(new QStandardItem(QStringLiteral("entry%1").arg(i)));

        FakeRemoteModelServer server(QStringLiteral("com.kdab.GammaRay.UnitTest.FilterModel"), this);
        server.setModel(&model);
        server.modelMonitored(true);

        FakeRemoteModel client(QStringLiteral("com.kdab.GammaRay.UnitTest.FilterModel"), this);
        connect(&server, &FakeRemoteModelServer::message, &client, &RemoteModel::newMessage);
        connect(&client, &FakeRemoteModel::message, &server, &RemoteModelServer::newRequest);

        QSortFilterProxyModel proxy;
        proxy.setSourceModel(&client);
        proxy.setFilterFixedString(QStringLiteral("entry"));
        proxy.sort(0);
        QAbstractItemModelTester modelTest(&proxy);

        QTRY_COMPARE(client.rowCount(), 10);
        for (int row = 0; row < client.rowCount(); ++row)
            QVERIFY(waitForData(client.index(row, 0)));
        QTRY_COMPARE(proxy.rowCount(), 10);

        for (int i = 0; i < 5; ++i)
            model.insertRow(i * 2, new QStandardItem(QStringLiteral("entry%1").arg(10 + i)));

        QTRY_COMPARE(proxy.rowCount(), 15);
        QCOMPARE(proxy.index(14, 0).data().toString(), QStringLiteral("entry9"));
    }

    void testRemoveColumns()
    {
        QStandardItemModel model(2, 3);
        for (int row = 0; row < 2; ++row) {
            for (int column = 0; column < 3; ++column)
                model.setItem(row, column, new QStandardItem(QStringLiteral("%1-%2").arg(row).arg(column)));
        }

        FakeRemoteModelServer server(QStringLiteral("com.kdab.GammaRay.UnitTest.ColumnModel"), this);
        server.setModel(&model);
        server.modelMonitored(true);

        FakeRemoteModel client(QStringLiteral("com.kdab.GammaRay.UnitTest.ColumnModel"), this);
        connect(&server, &FakeRemoteModelServer::message, &client, &RemoteModel::newMessage);
        connect(&client, &FakeRemoteModel::message, &server, &RemoteModelServer::newRequest);

        QAbstractItemModelTester modelTest(&client);
        QTRY_COMPARE(client.columnCount(), 3);
        for (int column = 0; column < 3; ++column)
            QVERIFY(waitForData(client.index(1, column)));

        model.removeColumn(1);
        QTRY_COMPARE(client.columnCount(), 2);
        QVERIFY(waitForData(client.index(1, 1)));
        QCOMPARE(client.index(1, 0).data().toString(), QStringLiteral("1-0"));
        QCOMPARE(client.index(1, 1).data().toString(), QStringLiteral("1-2"));
    }

    // the client must not trust row numbers that don't match what it knows
    void testSourceModelOutOfSync()
    {
        UnsignaledListModel listModel;
        for (int i = 0; i < 3; ++i)
            listModel.appendWithoutSignals(QString::number(i));

        FakeRemoteModelServer server(QStringLiteral("com.kdab.GammaRay.UnitTest.OutOfSyncModel"), this);
        server.setModel(&listModel);
        server.modelMonitored(true);

        FakeRemoteModel client(QStringLiteral("com.kdab.GammaRay.UnitTest.OutOfSyncModel"), this);
        connect(&server, &FakeRemoteModelServer::message, &client, &RemoteModel::newMessage);
        connect(&client, &FakeRemoteModel::message, &server, &RemoteModelServer::newRequest);

        QAbstractItemModelTester modelTest(&client);
        QTRY_COMPARE(client.rowCount(), 3);
        QCOMPARE(rowData(&client).size(), 3);

        for (int i = 3; i < 30; ++i)
            listModel.appendWithoutSignals(QString::number(i));
        // the client only knows 3 rows
        const auto outOfSyncGuard = FakeRemoteModel::allowOutOfSync();
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral("out of sync")));
        listModel.removeRowWithSignals(28);

        // the client starts over, and then shows what the source model has
        QTRY_COMPARE(client.rowCount(), 29);
        QCOMPARE(rowData(&client).size(), 29);
        QCOMPARE(rowData(&client).last(), QStringLiteral("29"));
    }
};

QTEST_MAIN(RemoteModelTest)

#include "remotemodeltest.moc"
