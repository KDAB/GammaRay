/*
  propertysyncertest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <common/propertysyncer.h>
#include <common/message.h>

#include <QBuffer>
#include <QObject>
#include <QTest>

using namespace GammaRay;

class MyObject : public QObject
{
    Q_PROPERTY(int intProp READ intProp WRITE setIntProp NOTIFY intPropChanged)
    Q_OBJECT
public:
    explicit MyObject(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
    int intProp()
    {
        return p1;
    }
    void setIntProp(int i)
    {
        if (p1 == i)
            return;
        p1 = i;
        emit intPropChanged();
    }

signals:
    void intPropChanged();

private:
    int p1 = 0;
};

class PropertySyncerTest : public QObject
{
    Q_OBJECT
public:
    explicit PropertySyncerTest(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

public slots:
    void server2client(const GammaRay::Message &msg)
    {
        ++m_server2ClientCount;
        if (!m_client)
            return;

        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::ReadWrite);
        msg.write(&buffer);
        buffer.seek(0);
        m_client->handleMessage(Message::readMessage(&buffer));
    }

    void client2server(const GammaRay::Message &msg)
    {
        ++m_client2ServerCount;
        Q_ASSERT(m_server);

        QByteArray ba;
        QBuffer buffer(&ba);
        buffer.open(QIODevice::ReadWrite);
        msg.write(&buffer);
        buffer.seek(0);
        m_server->handleMessage(Message::readMessage(&buffer));
    }

private slots:
    void testSync()
    {
        // server setup
        MyObject serverObj;
        serverObj.setIntProp(14);
        m_server = new PropertySyncer(this);
        connect(m_server, &PropertySyncer::message, this,
                &PropertySyncerTest::server2client);
        m_server->setAddress(1);
        m_server->addObject(42, &serverObj);

        // client setup
        auto *clientObj = new MyObject(this);
        m_client = new PropertySyncer(this);
        m_client->setRequestInitialSync(true);
        connect(m_client, &PropertySyncer::message, this,
                &PropertySyncerTest::client2server);
        m_client->setAddress(1);
        m_client->addObject(42, clientObj);
        m_server->setObjectEnabled(42, true);
        QCOMPARE(clientObj->intProp(), 0);
        QCOMPARE(m_client2ServerCount, 0);
        QCOMPARE(m_server2ClientCount, 0);

        // initial sync request
        m_client->setObjectEnabled(42, true);
        QCOMPARE(m_client2ServerCount, 1);
        QCOMPARE(m_server2ClientCount, 1);
        QCOMPARE(clientObj->intProp(), 14);

        // regular sync on changes on one side
        serverObj.setIntProp(42);
        QCOMPARE(m_server2ClientCount, 2);
        QCOMPARE(clientObj->intProp(), 42);

        QCOMPARE(m_client2ServerCount, 1);
        clientObj->setIntProp(23);
        QCOMPARE(serverObj.intProp(), 23);
        QCOMPARE(m_client2ServerCount, 2);
        QCOMPARE(m_server2ClientCount, 2);

        // client destroyed
        m_server->setObjectEnabled(42, false);
        delete clientObj;
        serverObj.setIntProp(26);
        QCOMPARE(m_server2ClientCount, 2);
    }

private:
    int m_server2ClientCount = 0, m_client2ServerCount = 0;
    PropertySyncer *m_client = nullptr;
    PropertySyncer *m_server = nullptr;
};

QTEST_MAIN(PropertySyncerTest)

#include "propertysyncertest.moc"
