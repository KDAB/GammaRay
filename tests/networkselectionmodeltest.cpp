/*
  networkselectionmodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <3rdparty/qt/modeltest.h>

#include <common/networkselectionmodel.h>
#include <common/message.h>
#include <common/endpoint.h>

#include <QBuffer>
#include <QDebug>
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QtEndian>
#include <QTest>

QT_BEGIN_NAMESPACE
namespace QTest {
template<>
bool qCompare(const QPersistentModelIndex &lhs, const QModelIndex &rhs,
              char const *actual, char const *expected, char const *file, int line)
{
    return qCompare(lhs, QPersistentModelIndex(rhs), actual, expected, file, line);
}
}
QT_END_NAMESPACE

using namespace GammaRay;

namespace GammaRay {
enum FakeAddress
{
    ServerAddress = 23,
    ClientAddress = 42,
};

class FakeEndpoint : public Endpoint
{
    Q_OBJECT
public:
    explicit FakeEndpoint(QObject *parent = nullptr)
        : Endpoint(parent)
    {
        setDevice(new QBuffer(this));
    }

protected:
    void doSendMessage(const Message &msg) override
    {
        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::ReadWrite);
        msg.write(&buffer);
        buffer.seek(sizeof(Protocol::PayloadSize));
        Protocol::ObjectAddress addr = qToBigEndian(static_cast<Protocol::ObjectAddress>(msg.address()
                                                                                                 == ServerAddress
                                                                                             ? ClientAddress
                                                                                             : ServerAddress));
        buffer.write(( char * )&addr, sizeof(addr));
        buffer.seek(0);
        emit message(Message::readMessage(&buffer));
    }

    bool isRemoteClient() const override
    {
        return true;
    }
    void messageReceived(const GammaRay::Message &) override
    {
    }
    QUrl serverAddress() const override
    {
        return QUrl();
    }
    void handlerDestroyed(Protocol::ObjectAddress, const QString &) override
    {
    }
    void objectDestroyed(Protocol::ObjectAddress, const QString &, QObject *) override
    {
    }

signals:
    void message(const GammaRay::Message &);
};

class FakeNetworkSelectionModel : public NetworkSelectionModel
{
    Q_OBJECT
public:
    explicit FakeNetworkSelectionModel(Protocol::ObjectAddress address, QAbstractItemModel *model,
                                       QObject *parent = nullptr)
        : NetworkSelectionModel(QStringLiteral("com.kdab.GammaRay.UnitTest.Model"), model, parent)
    {
        m_myAddress = address;

        auto endpoint = qobject_cast<FakeEndpoint *>(FakeEndpoint::instance());
        QVERIFY(endpoint);
        connect(endpoint, &FakeEndpoint::message, this, &FakeNetworkSelectionModel::dispatchMessage);
    }

    void applyPendingSelection()
    {
        NetworkSelectionModel::applyPendingSelection();
    }
    void requestSelection()
    {
        NetworkSelectionModel::requestSelection();
    }

private slots:
    void dispatchMessage(const GammaRay::Message &msg)
    {
        if (msg.address() != m_myAddress)
            return;
        QMetaObject::invokeMethod(this, "newMessage", Q_ARG(GammaRay::Message, msg));
    }
};
}

class NetworkSelectionModelTest : public QObject
{
    Q_OBJECT
private:
    static void fillModel(QStandardItemModel *model)
    {
        model->appendRow(new QStandardItem(QStringLiteral("Row 1")));
        model->appendRow(new QStandardItem(QStringLiteral("Row 2")));
        model->appendRow(new QStandardItem(QStringLiteral("Row 3")));
        model->appendRow(new QStandardItem(QStringLiteral("Row 4")));
        model->appendRow(new QStandardItem(QStringLiteral("Row 5")));
    }

private slots:
    static void initTestCase()
    {
        qRegisterMetaType<QItemSelection>();
        qRegisterMetaType<QModelIndex>();

        new FakeEndpoint;
        QVERIFY(Endpoint::instance());
        QVERIFY(Endpoint::isConnected());
    }

    static void cleanupTestCase()
    {
        delete FakeEndpoint::instance();
    }

    void testSelectionUpdate()
    {
        QStandardItemModel serverModel;
        FakeNetworkSelectionModel serverSelection(ServerAddress, &serverModel);
        fillModel(&serverModel);
        QSignalSpy serverSpy(&serverSelection, &FakeNetworkSelectionModel::selectionChanged);
        QVERIFY(serverSpy.isValid());

        QStandardItemModel clientModel;
        fillModel(&clientModel);
        FakeNetworkSelectionModel clientSelection(ClientAddress, &clientModel);
        QSignalSpy clientSpy(&clientSelection, &FakeNetworkSelectionModel::selectionChanged);
        QVERIFY(clientSpy.isValid());

        serverSelection.select(serverModel.index(2, 0), QItemSelectionModel::ClearAndSelect);
        QCOMPARE(serverSelection.selection().size(), 1);
        QCOMPARE(serverSelection.selection().first().topLeft(), serverModel.index(2, 0));
        QCOMPARE(clientSelection.selection().size(), 1);
        QCOMPARE(clientSelection.selection().first().topLeft(), clientModel.index(2, 0));
        QCOMPARE(clientSpy.size(), 1);

        serverSpy.clear();
        clientSelection.select(clientModel.index(4, 0), QItemSelectionModel::ClearAndSelect);
        QCOMPARE(clientSelection.selection().size(), 1);
        QCOMPARE(clientSelection.selection().first().topLeft(), clientModel.index(4, 0));
        QCOMPARE(serverSelection.selection().size(), 1);
        QCOMPARE(serverSelection.selection().first().topLeft(), serverModel.index(4, 0));
        QCOMPARE(serverSpy.size(), 1);
    }

    void testDelayedSelectionUpdate()
    {
        QStandardItemModel serverModel;
        FakeNetworkSelectionModel serverSelection(ServerAddress, &serverModel);
        fillModel(&serverModel);

        QStandardItemModel clientModel;
        FakeNetworkSelectionModel clientSelection(ClientAddress, &clientModel);
        QSignalSpy clientSpy(&clientSelection, &FakeNetworkSelectionModel::selectionChanged);
        QVERIFY(clientSpy.isValid());
        QVERIFY(!clientSelection.hasSelection());

        serverSelection.select(serverModel.index(4, 0), QItemSelectionModel::ClearAndSelect);
        QTest::qWait(100);
        fillModel(&clientModel);
        clientSelection.applyPendingSelection(); // usually triggered by SelectionModelClient

        QCOMPARE(clientSelection.selection().size(), 1);
        QCOMPARE(clientSelection.selection().first().topLeft(), clientModel.index(4, 0));
        QCOMPARE(clientSpy.size(), 1);
    }

    void testInitialSelectionTransfer()
    {
        QStandardItemModel serverModel;
        FakeNetworkSelectionModel serverSelection(ServerAddress, &serverModel);
        fillModel(&serverModel);
        serverSelection.select(serverModel.index(3, 0), QItemSelectionModel::ClearAndSelect);

        QTest::qWait(100);

        QStandardItemModel clientModel;
        fillModel(&clientModel);
        FakeNetworkSelectionModel clientSelection(ClientAddress, &clientModel);
        QSignalSpy clientSpy(&clientSelection, &FakeNetworkSelectionModel::selectionChanged);
        QVERIFY(clientSpy.isValid());

        clientSelection.requestSelection(); // usually called by SelectionModelClient

        QCOMPARE(clientSelection.selection().size(), 1);
        QCOMPARE(clientSelection.selection().first().topLeft(), clientModel.index(3, 0));
        QCOMPARE(clientSpy.size(), 1);
    }

    void testCurrent()
    {
        QStandardItemModel serverModel;
        FakeNetworkSelectionModel serverSelection(ServerAddress, &serverModel);
        fillModel(&serverModel);
        QSignalSpy serverSpy(&serverSelection, &FakeNetworkSelectionModel::selectionChanged);
        QVERIFY(serverSpy.isValid());

        QStandardItemModel clientModel;
        fillModel(&clientModel);
        FakeNetworkSelectionModel clientSelection(ClientAddress, &clientModel);
        QSignalSpy clientSpy(&clientSelection, &FakeNetworkSelectionModel::selectionChanged);
        QVERIFY(clientSpy.isValid());
        QSignalSpy clientRowSpy(&clientSelection, &FakeNetworkSelectionModel::currentRowChanged);
        QVERIFY(clientRowSpy.isValid());

        serverSelection.setCurrentIndex(serverModel.index(2, 0), QItemSelectionModel::NoUpdate);
        QVERIFY(!serverSelection.hasSelection());
        QCOMPARE(serverSelection.currentIndex(), serverModel.index(2, 0));
        QVERIFY(!clientSelection.hasSelection());
        QCOMPARE(clientSelection.currentIndex(), clientModel.index(2, 0));
        QCOMPARE(clientSpy.size(), 1);
        QCOMPARE(clientRowSpy.size(), 1);

        serverSpy.clear();
        clientSelection.setCurrentIndex(clientModel.index(4, 0), QItemSelectionModel::NoUpdate);
        QVERIFY(!clientSelection.hasSelection());
        QCOMPARE(clientSelection.currentIndex(), clientModel.index(4, 0));
        QVERIFY(!serverSelection.hasSelection());
        QCOMPARE(serverSelection.currentIndex(), serverModel.index(4, 0));
        QCOMPARE(serverSpy.size(), 1);
    }
};

QTEST_MAIN(NetworkSelectionModelTest)

#include "networkselectionmodeltest.moc"
