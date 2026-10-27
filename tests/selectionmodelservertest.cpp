/*
  selectionmodelservertest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2026 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "baseprobetest.h"

#include <common/message.h>
#include <common/objectbroker.h>
#include <common/protocol.h>
#include <core/remote/server.h>

#include <QBuffer>
#include <QItemSelectionModel>
#include <QSignalSpy>
#include <QStandardItemModel>

#include <functional>
#include <memory>

using namespace GammaRay;

static const char modelName[] = "com.kdab.GammaRay.UnitTest.SelectionModel";

// the server side of a selection model, with messages as a client would send them
class SelectionModelServerTest : public BaseProbeTest
{
    Q_OBJECT
private:
    QItemSelectionModel *selectionModel() const
    {
        return ObjectBroker::selectionModel(m_model.get());
    }

    // delivers the message like the endpoint does when it comes from the client
    void send(Protocol::MessageType type, const std::function<void(Message &)> &write) const
    {
        const auto address = Server::instance()->objectAddress(QString::fromLatin1(modelName) + QStringLiteral(".selection"));
        QVERIFY(address != Protocol::InvalidObjectAddress);
        Message msg(address, type);
        write(msg);
        QByteArray data;
        QBuffer buffer(&data);
        buffer.open(QIODevice::ReadWrite);
        msg.write(&buffer);
        buffer.seek(0);
        const auto received = Message::readMessage(&buffer);
        QVERIFY(QMetaObject::invokeMethod(selectionModel(), "newMessage", Qt::DirectConnection,
                                          Q_ARG(GammaRay::Message, received)));
    }

    void sendSelection(const QVector<std::pair<QModelIndex, QModelIndex>> &ranges, QItemSelectionModel::SelectionFlags command) const
    {
        send(Protocol::SelectionModelSelect, [&](Message &msg) {
            msg << qint32(ranges.size());
            for (const auto &range : ranges)
                msg << Protocol::fromQModelIndex(range.first) << Protocol::fromQModelIndex(range.second);
            msg << quint32(command);
        });
    }

    // an index the model doesn't have
    static Protocol::ModelIndex missingIndex(int row)
    {
        Protocol::ModelIndex index;
        index.push_back(Protocol::ModelIndexData(row, 0));
        return index;
    }

private slots:
    void initTestCase()
    {
        createProbe();
        // ObjectBroker owns the selection model, and keeps it for the model, so this one stays
        m_model = std::make_unique<QStandardItemModel>();
        m_model->setObjectName(QString::fromLatin1(modelName));
    }

    void init()
    {
        m_model->clear();
        selectionModel()->clear();
        for (int row = 0; row < 3; ++row) {
            auto item = new QStandardItem(QStringLiteral("item %1").arg(row));
            for (int child = 0; child < 2; ++child)
                item->appendRow(new QStandardItem(QStringLiteral("child %1-%2").arg(row).arg(child)));
            m_model->appendRow(item);
        }
    }

    // QTest::qExec() doesn't emit aboutToQuit(), so the probe would only go away with the statics
    // it depends on
    void cleanupTestCase()
    {
        delete Probe::instance();
        m_model.reset();
    }

    // the probe creates selection models that the client can talk to
    void testCreatedByObjectBroker()
    {
        auto selection = selectionModel();
        QCOMPARE(selection->metaObject()->className(), "GammaRay::SelectionModelServer"); // not exported
        QCOMPARE(selection->model(), m_model.get());
        QCOMPARE(selectionModel(), selection);
        QVERIFY(Server::instance()->objectAddress(QString::fromLatin1(modelName) + QStringLiteral(".selection")) != Protocol::InvalidObjectAddress);
    }

    void testSelectFromClient()
    {
        QSignalSpy spy(selectionModel(), &QItemSelectionModel::selectionChanged);
        const auto row = m_model->index(1, 0);
        const auto child = m_model->index(0, 0, m_model->index(2, 0));
        sendSelection({ { row, row }, { child, child } }, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
        QCOMPARE(spy.size(), 1);
        QVERIFY(selectionModel()->isSelected(row));
        QVERIFY(selectionModel()->isSelected(child));
        QVERIFY(!selectionModel()->isSelected(m_model->index(0, 0)));

        // and replace it again
        sendSelection({ { m_model->index(0, 0), m_model->index(0, 0) } }, QItemSelectionModel::ClearAndSelect);
        QVERIFY(selectionModel()->isSelected(m_model->index(0, 0)));
        QVERIFY(!selectionModel()->isSelected(row));
        QVERIFY(!selectionModel()->isSelected(child));
    }

    // e.g. the rows were removed meanwhile
    void testSelectMissingRows()
    {
        selectionModel()->select(m_model->index(1, 0), QItemSelectionModel::ClearAndSelect);
        QSignalSpy spy(selectionModel(), &QItemSelectionModel::selectionChanged);
        send(Protocol::SelectionModelSelect, [](Message &msg) {
            msg << qint32(1) << missingIndex(42) << missingIndex(42) << quint32(QItemSelectionModel::ClearAndSelect);
        });
        QCOMPARE(spy.size(), 0);
        QVERIFY(selectionModel()->isSelected(m_model->index(1, 0)));

        // a later selection still works
        sendSelection({ { m_model->index(2, 0), m_model->index(2, 0) } }, QItemSelectionModel::ClearAndSelect);
        QVERIFY(selectionModel()->isSelected(m_model->index(2, 0)));
        QVERIFY(!selectionModel()->isSelected(m_model->index(1, 0)));
    }

    void testCurrentFromClient()
    {
        const auto child = m_model->index(1, 0, m_model->index(0, 0));
        send(Protocol::SelectionModelCurrent, [&](Message &msg) {
            msg << quint32(QItemSelectionModel::Current) << Protocol::fromQModelIndex(child);
        });
        QCOMPARE(selectionModel()->currentIndex(), QModelIndex(child));

        send(Protocol::SelectionModelCurrent, [](Message &msg) {
            msg << quint32(QItemSelectionModel::Current) << missingIndex(42);
        });
        QCOMPARE(selectionModel()->currentIndex(), QModelIndex(child));
    }

    // without a client, changes of the model and the selection go nowhere
    void testMonitoredWithoutClient()
    {
        auto selection = selectionModel();
        QVERIFY(QMetaObject::invokeMethod(selection, "modelMonitored", Q_ARG(bool, true)));
        selection->select(m_model->index(0, 0), QItemSelectionModel::ClearAndSelect);
        m_model->appendRow(new QStandardItem(QStringLiteral("new")));
        m_model->insertRow(0, new QStandardItem(QStringLiteral("first")));
        QTest::qWait(200); // the server sends the selection after model changes
        QVERIFY(selection->isSelected(m_model->index(1, 0)));
        send(Protocol::SelectionModelStateRequest, [](Message &) {});

        QVERIFY(QMetaObject::invokeMethod(selection, "modelMonitored", Q_ARG(bool, false)));
        m_model->clear();
        QTest::qWait(200);
        QVERIFY(!selection->hasSelection());
    }

private:
    std::unique_ptr<QStandardItemModel> m_model;
};

QTEST_GUILESS_MAIN(SelectionModelServerTest)

#include "selectionmodelservertest.moc"
