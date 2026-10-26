/*
  multithreadingtest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "baseprobetest.h"

#include <core/probe.h>

#include <QDebug>
#include <QScopeGuard>
#include <QSignalSpy>
#include <QSortFilterProxyModel>
#include <QThread>

#include <functional>

using namespace GammaRay;

class Thread : public QThread
{
    Q_OBJECT
public:
    Thread() = default;

    void run() override
    {
        objects.reserve(batchSize);
        for (int i = 0; i < iterations; ++i) {
            for (int j = 0; j < batchSize; ++j) {
                objects.push_back(new QObject);
            }
            QTest::qWait(delay);
            qDeleteAll(objects);
            objects.clear();
        }
    }

    QVector<QObject *> objects;
    int batchSize = 1;
    int delay = 0;
    int iterations = 100;
};

class MultiThreadingTest : public BaseProbeTest
{
    Q_OBJECT
private slots:
    static void testCreateDestroy_data()
    {
        QTest::addColumn<int>("batchSize", nullptr);
        QTest::addColumn<int>("delay", nullptr);
        QTest::addColumn<int>("iterations", nullptr);

        QTest::newRow("10-0-1000") << 10 << 0 << 1000;
        QTest::newRow("100-1-100") << 100 << 1 << 100;
        QTest::newRow("1000-10-100") << 1000 << 10 << 100;
    }

    void testCreateDestroy()
    {
        QFETCH(int, batchSize);
        QFETCH(int, delay);
        QFETCH(int, iterations);

        createProbe();

        Thread t;
        t.batchSize = batchSize;
        t.delay = delay;
        t.iterations = iterations;
        QTest::qWait(1);

        QSignalSpy spy(&t, &Thread::finished);
        QVERIFY(spy.isValid());

        // this mainly aims at not triggering any of the sanity checks in the object models or Probe
        t.start();
        QVERIFY(spy.wait(30000));
    }

    // objects destroyed in another thread stay in the object tree until the destruction is
    // processed in the main thread, but they already show as "<deleted>"
    void testDeferredDestroyWithRecursiveFilter()
    {
        createProbe();
        auto model = Probe::instance()->objectTreeModel();

        QThread thread;
        thread.start();
        auto context = new QObject;
        context->moveToThread(&thread);
        const auto inThread = [context](const std::function<void()> &f) {
            QMetaObject::invokeMethod(context, f, Qt::BlockingQueuedConnection);
        };
        const auto find = [model](const QString &name) {
            return model->match(model->index(0, 0), Qt::DisplayRole, name, 1,
                                Qt::MatchFixedString | Qt::MatchRecursive)
                .value(0);
        };

        QObject *root = nullptr;
        QObject *match1 = nullptr;
        const auto cleanup = qScopeGuard([&] {
            inThread([&] {
                delete root;
                delete context;
            });
            thread.quit();
            thread.wait();
        });
        inThread([&] {
            root = new QObject;
            root->setObjectName(QStringLiteral("root"));
            match1 = new QObject(root);
            match1->setObjectName(QStringLiteral("match1"));
        });
        QTRY_VERIFY(find(QStringLiteral("match1")).isValid());

        // "root" itself is only accepted because of its children
        QSortFilterProxyModel proxy;
        proxy.setRecursiveFilteringEnabled(true);
        proxy.setAutoAcceptChildRows(true);
        proxy.setFilterFixedString(QStringLiteral("match"));
        proxy.setSourceModel(model);
        auto proxyRoot = proxy.mapFromSource(find(QStringLiteral("root")));
        QVERIFY(proxyRoot.isValid());
        QCOMPARE(proxy.rowCount(proxyRoot), 1);

        // queues the creation of match2 before the destruction of match1, but match1 is invalid
        // already when the main thread processes the creation
        inThread([&] {
            auto match2 = new QObject(root);
            match2->setObjectName(QStringLiteral("match2"));
            delete match1;
        });
        QTRY_VERIFY(find(QStringLiteral("match2")).isValid());
        QVERIFY(!find(QStringLiteral("match1")).isValid());
        QCOMPARE(model->rowCount(find(QStringLiteral("root"))), 1);

        proxyRoot = proxy.mapFromSource(find(QStringLiteral("root")));
        QVERIFY(proxyRoot.isValid());
        QCOMPARE(proxy.rowCount(proxyRoot), 1);
        QCOMPARE(proxy.index(0, 0, proxyRoot).data().toString(), QStringLiteral("match2"));
    }
};

QTEST_MAIN(MultiThreadingTest)

#include "multithreadingtest.moc"
