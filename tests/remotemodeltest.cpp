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
#include <QTimer>
#include <QIcon>
#include <QPixmap>


#include <common/objectmodel.h>
#include <common/sourcelocation.h>

#include <memory>

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

    void changeRowsWithSignals(int first, int last)
    {
        emit dataChanged(index(first), index(last));
    }

    void insertRowWithSignals(int row, const QString &data)
    {
        beginInsertRows({}, row, row);
        m_rows.insert(row, data);
        endInsertRows();
    }

    void moveRowWithSignals(int from, int to)
    {
        beginMoveRows({}, from, from, {}, to);
        m_rows.move(from, to > from ? to - 1 : to);
        endMoveRows();
    }

private:
    QStringList m_rows;
};

// groups of rows on the top level, whose children can move between them
class GroupModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit GroupModel(const QList<QStringList> &groups)
        : m_groups(groups)
    {
    }

    QModelIndex index(int row, int column, const QModelIndex &parent = {}) const override
    {
        if (!hasIndex(row, column, parent))
            return {};
        // the internal id is 0 for groups, and the group's row + 1 for their children
        return createIndex(row, column, parent.isValid() ? quintptr(parent.row() + 1) : quintptr(0));
    }

    QModelIndex parent(const QModelIndex &child) const override
    {
        if (!child.isValid() || child.internalId() == 0)
            return {};
        return createIndex(int(child.internalId() - 1), 0, quintptr(0));
    }

    int rowCount(const QModelIndex &parent = {}) const override
    {
        if (!parent.isValid())
            return m_groups.size();
        if (parent.internalId() == 0 && parent.column() == 0)
            return m_groups.at(parent.row()).size();
        return 0;
    }

    int columnCount(const QModelIndex & = {}) const override
    {
        return 1;
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        if (role != Qt::DisplayRole)
            return {};
        if (index.internalId() == 0)
            return QStringLiteral("group%1").arg(index.row());
        return m_groups.at(int(index.internalId() - 1)).at(index.row());
    }

    void moveChild(int fromGroup, int row, int toGroup, int destination)
    {
        QVERIFY(beginMoveRows(index(fromGroup, 0), row, row, index(toGroup, 0), destination));
        const auto data = m_groups[fromGroup].takeAt(row);
        m_groups[toGroup].insert(fromGroup == toGroup && destination > row ? destination - 1 : destination, data);
        endMoveRows();
    }

private:
    QList<QStringList> m_groups;
};

// a single row with columns that can be moved
class ColumnModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit ColumnModel(const QStringList &columns)
        : m_columns(columns)
    {
    }

    int rowCount(const QModelIndex &parent = {}) const override
    {
        return parent.isValid() ? 0 : 1;
    }

    int columnCount(const QModelIndex &parent = {}) const override
    {
        return parent.isValid() ? 0 : m_columns.size();
    }

    QVariant data(const QModelIndex &index, int role) const override
    {
        return role == Qt::DisplayRole ? QVariant(m_columns.at(index.column())) : QVariant();
    }

    void moveColumnToFront(int column)
    {
        QVERIFY(beginMoveColumns({}, column, column, {}, 0));
        m_columns.move(column, 0);
        endMoveColumns();
    }

private:
    QStringList m_columns;
};

class RemoteModelTest : public QObject
{
    Q_OBJECT
private:
    // a server for model and a client connected to it
    struct Connection
    {
        // the model tester requests data on its own
        explicit Connection(QAbstractItemModel *model, bool withModelTester = true)
            : server(QStringLiteral("com.kdab.GammaRay.UnitTest.Model"))
            , client(QStringLiteral("com.kdab.GammaRay.UnitTest.Model"))
        {
            server.setModel(model);
            server.modelMonitored(true);
            QObject::connect(&server, &FakeRemoteModelServer::message, &client, &RemoteModel::newMessage);
            QObject::connect(&client, &FakeRemoteModel::message, &server, &RemoteModelServer::newRequest);
            if (withModelTester)
                modelTest = std::make_unique<QAbstractItemModelTester>(&client);
        }

        FakeRemoteModelServer server;
        FakeRemoteModel client;
        std::unique_ptr<QAbstractItemModelTester> modelTest;
    };

    static QString header(QAbstractItemModel *model, int section, Qt::Orientation orientation)
    {
        return model->headerData(section, orientation).toString(); // triggers fetching it
    }

    // fetches the data of all children of parent
    static QStringList childData(const QAbstractItemModel *model, const QModelIndex &parent)
    {
        QStringList rows;
        for (int row = 0; row < model->rowCount(parent); ++row) {
            const auto index = model->index(row, 0, parent);
            if (!waitForData(index))
                return {};
            rows.push_back(index.data().toString());
        }
        return rows;
    }

    // counts the messages the client sends from now on, by type
    static std::shared_ptr<QHash<int, int>> countRequests(FakeRemoteModel *client)
    {
        auto counts = std::make_shared<QHash<int, int>>();
        QObject::connect(client, &FakeRemoteModel::message, client, [counts](const GammaRay::Message &msg) {
            ++(*counts)[msg.type()];
        });
        return counts;
    }

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

    void testDataChanged()
    {
        QStandardItemModel model;
        for (int i = 0; i < 3; ++i)
            model.appendRow(new QStandardItem(QStringLiteral("entry%1").arg(i)));
        Connection c(&model);
        QTRY_COMPARE(c.client.rowCount(), 3);
        QCOMPARE(rowData(&c.client), QStringList({ "entry0", "entry1", "entry2" }));

        QSignalSpy spy(&c.client, &QAbstractItemModel::dataChanged);
        model.item(1)->setText(QStringLiteral("changed"));
        QTRY_COMPARE(rowData(&c.client), QStringList({ "entry0", "changed", "entry2" }));
        QVERIFY(!spy.isEmpty());
    }

    void testSetData()
    {
        QStandardItemModel model;
        model.appendRow(new QStandardItem(QStringLiteral("entry0")));
        Connection c(&model);
        QTRY_COMPARE(c.client.rowCount(), 1);
        QVERIFY(waitForData(c.client.index(0, 0)));

        c.client.setData(c.client.index(0, 0), QStringLiteral("edited"), Qt::EditRole);
        QTRY_COMPARE(model.item(0)->text(), QStringLiteral("edited"));
        QTRY_COMPARE(c.client.index(0, 0).data().toString(), QStringLiteral("edited"));
    }

    void testFlags()
    {
        QStandardItemModel model;
        auto item = new QStandardItem(QStringLiteral("entry0"));
        item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsDragEnabled);
        model.appendRow(item);
        model.appendRow(new QStandardItem(QStringLiteral("entry1")));
        Connection c(&model);
        QTRY_COMPARE(c.client.rowCount(), 2);
        QVERIFY(waitForData(c.client.index(0, 0)));
        QVERIFY(waitForData(c.client.index(1, 0)));

        QCOMPARE(c.client.flags(c.client.index(0, 0)), model.flags(model.index(0, 0)));
        QCOMPARE(c.client.flags(c.client.index(1, 0)), model.flags(model.index(1, 0)));
    }

    void testHeaders()
    {
        QStandardItemModel model(2, 3);
        model.setHorizontalHeaderLabels({ "A", "B", "C" });
        model.setVerticalHeaderLabels({ "first", "second" });
        Connection c(&model);
        QTRY_COMPARE(c.client.columnCount(), 3);

        QTRY_COMPARE(header(&c.client, 1, Qt::Horizontal), QStringLiteral("B"));
        QTRY_COMPARE(header(&c.client, 2, Qt::Horizontal), QStringLiteral("C"));
        QTRY_COMPARE(header(&c.client, 1, Qt::Vertical), QStringLiteral("second"));

        model.setHeaderData(1, Qt::Horizontal, QStringLiteral("changed"));
        QTRY_COMPARE(header(&c.client, 1, Qt::Horizontal), QStringLiteral("changed"));
        QCOMPARE(header(&c.client, 2, Qt::Horizontal), QStringLiteral("C"));
    }

    // the server answers the request after it removed the section
    void testHeaderReplyAfterColumnsRemoved()
    {
        QStandardItemModel model(2, 3);
        model.setHorizontalHeaderLabels({ "A", "B", "C" });
        Connection c(&model);
        QTRY_COMPARE(c.client.columnCount(), 3);

        model.removeColumn(2);
        header(&c.client, 2, Qt::Horizontal);
        QTRY_COMPARE(c.client.columnCount(), 2);
        QTRY_COMPARE(header(&c.client, 1, Qt::Horizontal), QStringLiteral("B"));
    }

    void testInsertColumns()
    {
        QStandardItemModel model(2, 2);
        for (int row = 0; row < 2; ++row) {
            for (int column = 0; column < 2; ++column)
                model.setItem(row, column, new QStandardItem(QStringLiteral("%1-%2").arg(row).arg(column)));
        }
        Connection c(&model);
        QTRY_COMPARE(c.client.columnCount(), 2);
        QVERIFY(waitForData(c.client.index(1, 0)));
        QVERIFY(waitForData(c.client.index(1, 1)));

        model.insertColumn(1, { new QStandardItem(QStringLiteral("new0")), new QStandardItem(QStringLiteral("new1")) });
        QTRY_COMPARE(c.client.columnCount(), 3);
        QVERIFY(waitForData(c.client.index(1, 1)));
        QCOMPARE(c.client.index(1, 0).data().toString(), QStringLiteral("1-0"));
        QCOMPARE(c.client.index(1, 1).data().toString(), QStringLiteral("new1"));
        QCOMPARE(c.client.index(1, 2).data().toString(), QStringLiteral("1-1"));
    }

    void testChildRows()
    {
        QStandardItemModel model;
        auto parent = new QStandardItem(QStringLiteral("parent"));
        parent->appendRow(new QStandardItem(QStringLiteral("child0")));
        model.appendRow(parent);
        Connection c(&model);
        QTRY_COMPARE(c.client.rowCount(), 1);
        const auto parentIndex = c.client.index(0, 0);
        QTRY_COMPARE(c.client.rowCount(parentIndex), 1);

        parent->appendRow(new QStandardItem(QStringLiteral("child1")));
        parent->insertRow(0, new QStandardItem(QStringLiteral("child-1")));
        QTRY_COMPARE(c.client.rowCount(parentIndex), 3);
        QVERIFY(waitForData(c.client.index(0, 0, parentIndex)));
        QVERIFY(waitForData(c.client.index(2, 0, parentIndex)));
        QCOMPARE(c.client.index(0, 0, parentIndex).data().toString(), QStringLiteral("child-1"));
        QCOMPARE(c.client.index(2, 0, parentIndex).data().toString(), QStringLiteral("child1"));
        QCOMPARE(c.client.parent(c.client.index(2, 0, parentIndex)), parentIndex);

        parent->removeRow(1);
        QTRY_COMPARE(c.client.rowCount(parentIndex), 2);
        QCOMPARE(c.client.index(1, 0, parentIndex).data().toString(), QStringLiteral("child1"));
    }

    // a sort is a layout change
    void testSortOnServer()
    {
        QStandardItemModel model;
        for (const auto &text : { "c", "a", "b" })
            model.appendRow(new QStandardItem(QString::fromLatin1(text)));
        Connection c(&model);
        QTRY_COMPARE(c.client.rowCount(), 3);
        QCOMPARE(rowData(&c.client), QStringList({ "c", "a", "b" }));

        c.client.sort(0, Qt::AscendingOrder);
        QTRY_COMPARE(rowData(&c.client), QStringList({ "a", "b", "c" }));
    }

    void testModelReset()
    {
        QStringListModel model({ "a", "b" });
        Connection c(&model);
        QTRY_COMPARE(c.client.rowCount(), 2);
        QCOMPARE(rowData(&c.client), QStringList({ "a", "b" }));

        model.setStringList({ "x", "y", "z" });
        QTRY_COMPARE(c.client.rowCount(), 3);
        QCOMPARE(rowData(&c.client), QStringList({ "x", "y", "z" }));
    }

    void testDataChangedOutOfSync()
    {
        UnsignaledListModel model;
        for (int i = 0; i < 3; ++i)
            model.appendWithoutSignals(QString::number(i));
        Connection c(&model);
        QTRY_COMPARE(c.client.rowCount(), 3);
        QCOMPARE(rowData(&c.client).size(), 3);

        for (int i = 3; i < 30; ++i)
            model.appendWithoutSignals(QString::number(i));
        const auto outOfSyncGuard = FakeRemoteModel::allowOutOfSync();
        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral("out of sync")));
        model.changeRowsWithSignals(1, 28);

        QTRY_COMPARE(c.client.rowCount(), 30);
    }

    // waiting for the source location is a nested event loop, which also processes other changes
    void testSourceLocationDuringReset()
    {
        QStringListModel model({ "a", "b" });
        Connection c(&model);
        QTRY_COMPARE(c.client.rowCount(), 2);
        QVERIFY(waitForData(c.client.index(0, 0)));

        // the reset makes the client ignore the answer, as it's from before the reset
        model.setStringList({ "a", "b" });
        bool timedOut = false;
        QTimer::singleShot(5000, &c.client, [&] {
            timedOut = true;
            emit c.client.declarationCreationLocationsReceived({}, {});
        });
        c.client.index(0, 0).data(ObjectModel::DeclarationLocationRole);
        QVERIFY(!timedOut);
        QTRY_COMPARE(rowData(&c.client), QStringList({ "a", "b" }));
    }

    void testSourceLocationRowRemoved()
    {
        QStringListModel model({ "a", "b", "c" });
        Connection c(&model);
        QTRY_COMPARE(c.client.rowCount(), 3);
        QVERIFY(waitForData(c.client.index(0, 0)));

        // processed while waiting for the answer
        model.removeRows(0, 1);
        const auto location = c.client.index(0, 0).data(ObjectModel::DeclarationLocationRole);
        QVERIFY(!location.value<SourceLocation>().isValid());
        QTRY_COMPARE(c.client.rowCount(), 2);
        QCOMPARE(rowData(&c.client), QStringList({ "b", "c" }));
    }

    static void testMoveRowsBetweenParents_data()
    {
        QTest::addColumn<bool>("sourceKnown");
        QTest::addColumn<bool>("destinationKnown");

        QTest::newRow("both known") << true << true;
        QTest::newRow("only source known") << true << false;
        QTest::newRow("only destination known") << false << true;
        QTest::newRow("none known") << false << false;
    }

    // the client only follows the changes to the parts it loaded already
    void testMoveRowsBetweenParents()
    {
        QFETCH(bool, sourceKnown);
        QFETCH(bool, destinationKnown);

        GroupModel model({ { "a0", "a1", "a2" }, { "b0", "b1" } });
        Connection c(&model);
        QTRY_COMPARE(c.client.rowCount(), 2);
        const auto group0 = c.client.index(0, 0);
        const auto group1 = c.client.index(1, 0);
        if (sourceKnown)
            QTRY_COMPARE(childData(&c.client, group0), QStringList({ "a0", "a1", "a2" }));
        if (destinationKnown)
            QTRY_COMPARE(childData(&c.client, group1), QStringList({ "b0", "b1" }));

        model.moveChild(0, 1, 1, 1);
        QTRY_COMPARE(childData(&c.client, group0), QStringList({ "a0", "a2" }));
        QTRY_COMPARE(childData(&c.client, group1), QStringList({ "b0", "a1", "b1" }));
        QCOMPARE(c.client.parent(c.client.index(1, 0, group1)), group1);
    }

    // the client doesn't support this, and starts over instead
    void testMoveColumns()
    {
        ColumnModel model({ "A", "B", "C" });
        Connection c(&model);
        QTRY_COMPARE(c.client.columnCount(), 3);
        for (int column = 0; column < 3; ++column)
            QVERIFY(waitForData(c.client.index(0, column)));

        QTest::ignoreMessage(QtWarningMsg, QRegularExpression(QStringLiteral("not implemented")));
        QSignalSpy resetSpy(&c.client, &QAbstractItemModel::modelReset);
        model.moveColumnToFront(2);
        QTRY_COMPARE(resetSpy.size(), 1);
        QTRY_COMPARE(c.client.columnCount(), 3);
        for (int column = 0; column < 3; ++column)
            QVERIFY(waitForData(c.client.index(0, column)));
        QCOMPARE(c.client.index(0, 0).data().toString(), QStringLiteral("C"));
        QCOMPARE(c.client.index(0, 2).data().toString(), QStringLiteral("B"));
    }

    // sorting the children of one item only changes the layout below it
    void testLayoutChangeOfChildren()
    {
        QStandardItemModel model;
        auto parent = new QStandardItem(QStringLiteral("parent"));
        for (const auto &text : { "c", "a", "b" })
            parent->appendRow(new QStandardItem(QString::fromLatin1(text)));
        model.appendRow(parent);
        model.appendRow(new QStandardItem(QStringLiteral("other")));
        Connection c(&model);
        QTRY_COMPARE(c.client.rowCount(), 2);
        const auto parentIndex = c.client.index(0, 0);
        QTRY_COMPARE(childData(&c.client, parentIndex), QStringList({ "c", "a", "b" }));
        QVERIFY(waitForData(c.client.index(1, 0)));
        const QPersistentModelIndex other(c.client.index(1, 0));

        parent->sortChildren(0);
        QTRY_COMPARE(childData(&c.client, parentIndex), QStringList({ "a", "b", "c" }));
        QVERIFY(other.isValid());
        QCOMPARE(other.data().toString(), QStringLiteral("other"));
    }

    void testServerUnregistered()
    {
        QStandardItemModel model;
        model.appendRow(new QStandardItem(QStringLiteral("entry0")));
        Connection c(&model);
        QTRY_COMPARE(c.client.rowCount(), 1);
        QVERIFY(waitForData(c.client.index(0, 0)));

        QSignalSpy resetSpy(&c.client, &QAbstractItemModel::modelReset);
        c.client.serverUnregistered(QStringLiteral("com.kdab.GammaRay.UnitTest.Model"), 42);
        QCOMPARE(resetSpy.size(), 1);
        QVERIFY(!c.client.isConnected());
        QCOMPARE(c.client.rowCount(), 0);
        QCOMPARE(c.client.columnCount(), 0);
        QVERIFY(!c.client.index(0, 0).isValid());
    }

    // cached headers have to move with the rows and columns
    void testHeadersFollowStructureChanges()
    {
        QStandardItemModel model(3, 3);
        model.setHorizontalHeaderLabels({ "A", "B", "C" });
        model.setVerticalHeaderLabels({ "r0", "r1", "r2" });
        Connection c(&model);
        QTRY_COMPARE(c.client.columnCount(), 3);
        for (int section = 0; section < 3; ++section) {
            QTRY_COMPARE(header(&c.client, section, Qt::Horizontal), model.headerData(section, Qt::Horizontal).toString());
            QTRY_COMPARE(header(&c.client, section, Qt::Vertical), model.headerData(section, Qt::Vertical).toString());
        }

        model.insertRow(0);
        model.setVerticalHeaderItem(0, new QStandardItem(QStringLiteral("new row")));
        model.removeColumn(1);
        QTRY_COMPARE(c.client.rowCount(), 4);
        QTRY_COMPARE(c.client.columnCount(), 2);
        QTRY_COMPARE(header(&c.client, 0, Qt::Vertical), QStringLiteral("new row"));
        QTRY_COMPARE(header(&c.client, 1, Qt::Vertical), QStringLiteral("r0"));
        QTRY_COMPARE(header(&c.client, 3, Qt::Vertical), QStringLiteral("r2"));
        QTRY_COMPARE(header(&c.client, 1, Qt::Horizontal), QStringLiteral("C"));
    }

    // the client knows there are no children without asking
    void testItemNeverHasChildren()
    {
        QStandardItemModel model;
        auto item = new QStandardItem(QStringLiteral("leaf"));
        item->setFlags(item->flags() | Qt::ItemNeverHasChildren);
        model.appendRow(item);
        Connection c(&model);
        QTRY_COMPARE(c.client.rowCount(), 1);
        QVERIFY(waitForData(c.client.index(0, 0)));

        const auto requests = countRequests(&c.client);
        QCOMPARE(c.client.rowCount(c.client.index(0, 0)), 0);
        QVERIFY(!c.client.hasChildren(c.client.index(0, 0)));
        QTest::qWait(10);
        QCOMPARE(requests->value(Protocol::ModelRowColumnCountRequest), 0);
    }

    // data requests are sent together, not one message per cell
    void testRequestBatching()
    {
        QStandardItemModel model;
        for (int i = 0; i < 250; ++i)
            model.appendRow(new QStandardItem(QStringLiteral("entry%1").arg(i)));
        Connection c(&model);
        QTRY_COMPARE(c.client.rowCount(), 250);

        const auto requests = countRequests(&c.client);
        for (int row = 0; row < 250; ++row)
            c.client.index(row, 0).data();
        QTRY_COMPARE(c.client.index(249, 0).data().toString(), QStringLiteral("entry249"));
        QCOMPARE(c.client.index(0, 0).data().toString(), QStringLiteral("entry0"));
        const int requestCount = requests->value(Protocol::ModelContentRequest);
        QVERIFY(requestCount > 0);
        QVERIFY2(requestCount <= 3, qPrintable(QString::number(requestCount)));
    }

    // the placeholder shown before the data arrives, which item views ask for a lot
    void testPlaceholderData()
    {
        QStandardItemModel model;
        model.appendRow(new QStandardItem(QStringLiteral("entry0")));
        Connection c(&model, false);
        QTRY_COMPARE(c.client.rowCount(), 1);
        const auto index = c.client.index(0, 0);

        // item views use the size hint for the layout, that shouldn't load anything
        index.data(Qt::SizeHintRole);
        QCOMPARE(index.data(RemoteModelRole::LoadingState).value<RemoteModelNodeState::NodeStates>(), RemoteModelNodeState::Empty | RemoteModelNodeState::Outdated);

        QVERIFY(!index.data().toString().isEmpty());
        QVERIFY(!index.data(Qt::ToolTipRole).isValid());
        QVERIFY(index.data(RemoteModelRole::LoadingState).value<RemoteModelNodeState::NodeStates>() & RemoteModelNodeState::Loading);
        QVERIFY(waitForData(index));
        QCOMPARE(index.data().toString(), QStringLiteral("entry0"));
    }

    // the server only sends what it can serialize
    void testDataTypes()
    {
        QStandardItemModel model;
        auto item = new QStandardItem(QStringLiteral("entry0"));
        item->setToolTip(QStringLiteral("tooltip"));
        item->setData(42, Qt::UserRole);
        item->setData(QVariantList { 1, QStringLiteral("two") }, Qt::UserRole + 1);
        item->setData(QVariant::fromValue<QObject *>(this), Qt::UserRole + 2);
        item->setData(QVariantList { 1, QVariant::fromValue<QObject *>(this) }, Qt::UserRole + 3);
        QPixmap pixmap(8, 8);
        pixmap.fill(Qt::red);
        item->setIcon(QIcon(pixmap));
        model.appendRow(item);
        Connection c(&model);
        QTRY_COMPARE(c.client.rowCount(), 1);
        const auto index = c.client.index(0, 0);
        QVERIFY(waitForData(index));

        QCOMPARE(index.data().toString(), QStringLiteral("entry0"));
        QCOMPARE(index.data(Qt::ToolTipRole).toString(), QStringLiteral("tooltip"));
        QCOMPARE(index.data(Qt::UserRole).toInt(), 42);
        QCOMPARE(index.data(Qt::UserRole + 1).toList(), QVariantList({ 1, QStringLiteral("two") }));
        QVERIFY(!index.data(Qt::UserRole + 2).isValid());
        QVERIFY(!index.data(Qt::UserRole + 3).isValid());
        // icons are sent as pixmaps
        QCOMPARE(index.data(Qt::DecorationRole).userType(), qMetaTypeId<QPixmap>());
        QCOMPARE(index.data(Qt::DecorationRole).value<QPixmap>().size(), QSize(8, 8));
    }

    void testProxyProperties()
    {
        QStandardItemModel model;
        Connection c(&model);

        QSignalSpy regExpSpy(&c.client, &RemoteModel::proxyFilterRegExpChanged);
        QSignalSpy keyColumnSpy(&c.client, &RemoteModel::proxyFilterKeyColumnChanged);
        QSignalSpy caseSpy(&c.client, &RemoteModel::proxyFilterCaseSensitivityChanged);
        QSignalSpy dynamicSpy(&c.client, &RemoteModel::proxyDynamicSortFilterChanged);

        QVERIFY(c.client.setProperty("filterRegularExpression", QRegularExpression(QStringLiteral("foo"))));
        QVERIFY(c.client.setProperty("filterKeyColumn", 2));
        QVERIFY(c.client.setProperty("filterCaseSensitivity", Qt::CaseInsensitive));
        QVERIFY(c.client.setProperty("dynamicSortFilter", true));
        QCOMPARE(c.client.property("filterRegularExpression").toRegularExpression().pattern(), QStringLiteral("foo"));
        QCOMPARE(c.client.property("filterKeyColumn").toInt(), 2);
        QCOMPARE(c.client.property("filterCaseSensitivity").value<Qt::CaseSensitivity>(), Qt::CaseInsensitive);
        QCOMPARE(c.client.property("dynamicSortFilter").toBool(), true);

        // setting the same values again doesn't notify
        c.client.setProperty("filterRegularExpression", QRegularExpression(QStringLiteral("foo")));
        c.client.setProperty("filterKeyColumn", 2);
        c.client.setProperty("filterCaseSensitivity", Qt::CaseInsensitive);
        c.client.setProperty("dynamicSortFilter", true);
        QCOMPARE(regExpSpy.size(), 1);
        QCOMPARE(keyColumnSpy.size(), 1);
        QCOMPARE(caseSpy.size(), 1);
        QCOMPARE(dynamicSpy.size(), 1);
    }

    static void testSourceModelOutOfSync_data()
    {
        QTest::addColumn<QString>("change");
        QTest::newRow("remove") << QStringLiteral("remove");
        QTest::newRow("insert") << QStringLiteral("insert");
        QTest::newRow("move") << QStringLiteral("move");
    }

    // the client must not trust row numbers that don't match what it knows
    void testSourceModelOutOfSync()
    {
        QFETCH(QString, change);

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
        QStringList expected;
        for (int i = 0; i < 30; ++i)
            expected.push_back(QString::number(i));
        if (change == QLatin1String("remove")) {
            listModel.removeRowWithSignals(28);
            expected.removeAt(28);
        } else if (change == QLatin1String("insert")) {
            listModel.insertRowWithSignals(28, QStringLiteral("new"));
            expected.insert(28, QStringLiteral("new"));
        } else {
            listModel.moveRowWithSignals(28, 0);
            expected.move(28, 0);
        }

        // the client starts over, and then shows what the source model has
        QTRY_COMPARE(client.rowCount(), expected.size());
        QCOMPARE(rowData(&client), expected);
    }
};

QTEST_MAIN(RemoteModelTest)

#include "remotemodeltest.moc"
