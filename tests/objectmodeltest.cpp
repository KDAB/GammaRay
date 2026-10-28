/*
  objectmodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2026 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "baseprobetest.h"

#include <common/objectid.h>
#include <common/objectmodel.h>
#include <core/probe.h>

#include <QAbstractItemModelTester>
#include <QScopeGuard>
#include <QSignalSpy>
#include <QSortFilterProxyModel>
#include <QThread>

#include <functional>
#include <memory>
#include <utility>

using namespace GammaRay;

class DerivedObject : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
};

// tests the probe's object list and object tree models, while the model tester checks every change
class ObjectModelTest : public BaseProbeTest
{
    Q_OBJECT
private:
    QAbstractItemModel *listModel() const
    {
        return Probe::instance()->objectListModel();
    }

    QAbstractItemModel *treeModel() const
    {
        return Probe::instance()->objectTreeModel();
    }

    static QModelIndex find(QAbstractItemModel *model, QObject *obj)
    {
        return model->match(model->index(0, 0), ObjectModel::ObjectRole, QVariant::fromValue(obj), 1,
                            Qt::MatchExactly | Qt::MatchRecursive)
            .value(0);
    }

    // lets the event loop run, so the probe processes the object changes it queued
    static void processQueue()
    {
        for (int i = 0; i < 3; ++i)
            QTest::qWait(1);
    }

    void setupFilteredProxy(QSortFilterProxyModel *proxy) const
    {
        proxy->setRecursiveFilteringEnabled(true);
        proxy->setFilterKeyColumn(0);
        proxy->setFilterFixedString(QStringLiteral("match"));
        proxy->setSourceModel(treeModel());
    }

    // the proxy that followed all changes shows the same as a new one
    static bool compareProxies(const QAbstractItemModel *a, const QModelIndex &aParent,
                               const QSortFilterProxyModel *b, const QModelIndex &bParent)
    {
        const int rows = a->rowCount(aParent);
        if (rows != b->rowCount(bParent)) {
            qWarning() << "proxy row count mismatch" << rows << b->rowCount(bParent);
            return false;
        }
        for (int row = 0; row < rows; ++row) {
            const auto aIndex = a->index(row, 0, aParent);
            const auto bIndex = b->index(row, 0, bParent);
            const auto aSource = static_cast<const QSortFilterProxyModel *>(a)->mapToSource(aIndex);
            if (aSource != b->mapToSource(bIndex)) {
                qWarning() << "proxy maps row" << row << "to a different object";
                return false;
            }
            if (!compareProxies(a, aIndex, b, bIndex))
                return false;
        }
        return true;
    }

    bool filteredProxyIsCorrect() const
    {
        QSortFilterProxyModel freshProxy;
        setupFilteredProxy(&freshProxy);
        return compareProxies(&freshProxy, {}, m_filteredProxy.get(), {});
    }

private slots:
    void initTestCase()
    {
        createProbe();
        m_listTester = std::make_unique<QAbstractItemModelTester>(listModel());
        m_treeTester = std::make_unique<QAbstractItemModelTester>(treeModel());
        m_filteredProxy = std::make_unique<QSortFilterProxyModel>();
        setupFilteredProxy(m_filteredProxy.get());
        m_proxyTester = std::make_unique<QAbstractItemModelTester>(m_filteredProxy.get());
    }

    // QTest::qExec() doesn't emit aboutToQuit(), so the probe would only go away with the statics
    // it depends on
    void cleanupTestCase()
    {
        m_proxyTester.reset();
        m_filteredProxy.reset();
        m_treeTester.reset();
        m_listTester.reset();
        delete Probe::instance();
    }

    void testData()
    {
        auto parent = std::make_unique<QObject>();
        auto obj = new DerivedObject(parent.get());
        obj->setObjectName(QStringLiteral("named"));
        processQueue();

        for (auto model : { listModel(), treeModel() }) {
            const auto index = find(model, obj);
            QVERIFY(index.isValid());
            QCOMPARE(model->columnCount(index.parent()), 2);
            QVERIFY(index.data().toString().contains(QStringLiteral("named")));
            QCOMPARE(index.sibling(index.row(), 1).data().toString(), QStringLiteral("DerivedObject"));
            QCOMPARE(index.data(ObjectModel::ObjectRole).value<QObject *>(), obj);
            QCOMPARE(index.data(ObjectModel::ObjectIdRole).value<ObjectId>(), ObjectId(obj));
            QVERIFY(!index.data(Qt::ToolTipRole).toString().isEmpty());
            const auto itemData = model->itemData(index);
            QCOMPARE(itemData.value(ObjectModel::ObjectIdRole).value<ObjectId>(), ObjectId(obj));
            QVERIFY(model->headerData(0, Qt::Horizontal).isValid());
            QVERIFY(model->headerData(1, Qt::Horizontal).isValid());
        }
        // only the tree has parents
        QCOMPARE(find(treeModel(), obj).parent(), find(treeModel(), parent.get()));
        QVERIFY(!find(listModel(), obj).parent().isValid());
    }

    void testCreateDestroy()
    {
        auto root = new QObject;
        auto child = new QObject(root);
        auto grandChild = new QObject(child);
        processQueue();
        for (auto obj : { root, child, grandChild }) {
            QVERIFY(find(listModel(), obj).isValid());
            QVERIFY(find(treeModel(), obj).isValid());
        }
        QCOMPARE(find(treeModel(), grandChild).parent(), find(treeModel(), child));
        QCOMPARE(treeModel()->rowCount(find(treeModel(), root)), 1);

        const int listRows = listModel()->rowCount();
        delete child;
        QCOMPARE(listModel()->rowCount(), listRows - 2);
        QCOMPARE(treeModel()->rowCount(find(treeModel(), root)), 0);
        QVERIFY(!find(listModel(), child).isValid());
        QVERIFY(!find(treeModel(), grandChild).isValid());
        delete root;
        QVERIFY(!find(treeModel(), root).isValid());
    }

    void testReparent()
    {
        auto a = std::make_unique<QObject>();
        auto b = std::make_unique<QObject>();
        auto obj = new QObject(a.get());
        new QObject(obj);
        QSignalSpy moveSpy(treeModel(), &QAbstractItemModel::rowsMoved);
        processQueue();
        const int listRows = listModel()->rowCount();

        obj->setParent(b.get());
        processQueue();
        QCOMPARE(moveSpy.size(), 1);
        QCOMPARE(find(treeModel(), obj).parent(), find(treeModel(), b.get()));
        QCOMPARE(treeModel()->rowCount(find(treeModel(), obj)), 1); // with its child
        QCOMPARE(treeModel()->rowCount(find(treeModel(), a.get())), 0);
        QCOMPARE(listModel()->rowCount(), listRows);

        obj->setParent(nullptr);
        processQueue();
        QVERIFY(!find(treeModel(), obj).parent().isValid());
        delete obj;
    }

    void testFavorites()
    {
        auto obj = std::make_unique<QObject>();
        processQueue();
        const auto index = find(treeModel(), obj.get());
        QVERIFY(index.isValid());
        QVERIFY(!index.data(ObjectModel::IsFavoriteRole).toBool());

        QSignalSpy changeSpy(treeModel(), &QAbstractItemModel::dataChanged);
        Probe::instance()->markObjectAsFavorite(obj.get());
        QCOMPARE(changeSpy.size(), 1);
        QCOMPARE(changeSpy.at(0).at(0).toModelIndex(), index);
        QVERIFY(index.data(ObjectModel::IsFavoriteRole).toBool());
        QVERIFY(treeModel()->itemData(index).value(ObjectModel::IsFavoriteRole).toBool());

        Probe::instance()->removeObjectAsFavorite(obj.get());
        QCOMPARE(changeSpy.size(), 2);
        QVERIFY(!index.data(ObjectModel::IsFavoriteRole).toBool());

        // a favorite that is destroyed is no favorite anymore, not even at the same address
        Probe::instance()->markObjectAsFavorite(obj.get());
        auto address = obj.get();
        obj.reset();
        auto reused = std::make_unique<QObject>();
        processQueue();
        if (reused.get() == address)
            QVERIFY(!find(treeModel(), reused.get()).data(ObjectModel::IsFavoriteRole).toBool());
    }

    void testObjectsInOtherThreads()
    {
        QThread thread;
        thread.start();
        auto context = new QObject;
        context->moveToThread(&thread);
        const auto inThread = [context](const std::function<void()> &f) {
            QMetaObject::invokeMethod(context, f, Qt::BlockingQueuedConnection);
        };

        QObject *root = nullptr;
        QObject *child = nullptr;
        inThread([&] {
            root = new QObject;
            child = new QObject(root);
        });
        QTRY_VERIFY(find(treeModel(), child).isValid());
        QCOMPARE(find(treeModel(), child).parent(), find(treeModel(), root));
        QVERIFY(find(listModel(), root).isValid());

        // destroyed in the other thread, but not processed yet
        inThread([&] { delete child; });
        const auto index = find(treeModel(), child);
        if (index.isValid()) {
            QCOMPARE(index.sibling(index.row(), 1).data().toString(), QStringLiteral("<deleted>"));
            QVERIFY(!index.data(ObjectModel::ObjectRole).isValid());
        }
        const auto listIndex = find(listModel(), child);
        if (listIndex.isValid())
            QVERIFY(!listIndex.data().isValid());
        processQueue();
        QVERIFY(!find(treeModel(), child).isValid());
        QVERIFY(!find(listModel(), child).isValid());

        inThread([&] {
            delete root;
            delete context;
        });
        thread.quit();
        thread.wait();
        processQueue();
        QVERIFY(!find(treeModel(), root).isValid());
    }

    // another thread can reparent its objects any time, also while the tree adds or moves them
    void testReparentedInOtherThreadMeanwhile()
    {
        QThread thread;
        thread.start();
        // also when a check fails
        const auto stopThread = qScopeGuard([&thread] {
            thread.quit();
            thread.wait();
        });
        auto context = new QObject;
        context->moveToThread(&thread);
        const auto inThread = [context](const std::function<void()> &f) {
            QMetaObject::invokeMethod(context, f, Qt::BlockingQueuedConnection);
        };

        QObject *a = nullptr;
        QObject *b = nullptr;
        QObject *c = nullptr;
        inThread([&] {
            a = new QObject;
            b = new QObject;
            c = new QObject;
        });
        processQueue();

        // moves obj to the next parent in the thread, as soon as the tree adds or moves it. This waits
        // for the thread while the probe holds its lock, which the thread mustn't need meanwhile
        QObject *obj = nullptr;
        QObject *nextParent = nullptr;
        const auto objectOf = [](const QModelIndex &index) {
            return index.data(ObjectModel::ObjectRole).value<QObject *>();
        };
        const auto moveObj = [&] {
            auto parent = std::exchange(nextParent, nullptr);
            inThread([&] { obj->setParent(parent); });
        };
        QObject hookContext;
        connect(treeModel(), &QAbstractItemModel::rowsAboutToBeInserted, &hookContext,
                [&](const QModelIndex &parent) {
                    if (nextParent && objectOf(parent) == a)
                        moveObj();
                });
        connect(treeModel(), &QAbstractItemModel::rowsAboutToBeMoved, &hookContext,
                [&](const QModelIndex &sourceParent, int sourceRow) {
                    if (nextParent && objectOf(treeModel()->index(sourceRow, 0, sourceParent)) == obj)
                        moveObj();
                });
        // the row of obj is where the model says it is
        const auto treeIsConsistent = [&] {
            const auto index = find(treeModel(), obj);
            return index.isValid() && treeModel()->index(index.row(), 0, index.parent()) == index;
        };

        // moved to b while the tree adds it below a
        nextParent = b;
        inThread([&] { obj = new QObject(a); });
        processQueue();
        QVERIFY(!nextParent);
        QVERIFY(treeIsConsistent());

        // the tree only notices the move to b when a goes away, then obj moves to c meanwhile
        nextParent = c;
        inThread([&] { delete a; });
        processQueue();
        QVERIFY(!nextParent);
        QVERIFY(treeIsConsistent());
        QVERIFY(filteredProxyIsCorrect());

        inThread([&] {
            delete b;
            delete c;
            delete context;
        });
        processQueue();
    }

    // another thread destroys an object and creates a new one at the same address, which a child of
    // the old one moves to, before the probe processes any of that
    void testDestroyedParentAddressReused()
    {
        QThread thread;
        thread.start();
        const auto stopThread = qScopeGuard([&thread] {
            thread.quit();
            thread.wait();
        });
        auto context = new QObject;
        context->moveToThread(&thread);
        connect(&thread, &QThread::finished, context, &QObject::deleteLater);
        const auto inThread = [context](const std::function<void()> &f) {
            QMetaObject::invokeMethod(context, f, Qt::BlockingQueuedConnection);
        };

        QObject *oldParent = nullptr;
        QObject *child = nullptr;
        inThread([&] {
            oldParent = new QObject;
            child = new QObject(oldParent);
        });
        processQueue();
        QCOMPARE(find(treeModel(), child).parent(), find(treeModel(), oldParent));

        QObject *newParent = nullptr;
        inThread([&] {
            child->setParent(nullptr);
            delete oldParent;
            newParent = new QObject;
            child->setParent(newParent);
        });
        const auto cleanup = qScopeGuard([&] {
            inThread([&] { delete newParent; });
            processQueue();
        });
        if (newParent != oldParent)
            QSKIP("the new object didn't get the address of the old one");

        processQueue();
        const auto index = find(treeModel(), child);
        QVERIFY(index.isValid());
        QCOMPARE(index.parent(), find(treeModel(), newParent));
        QCOMPARE(treeModel()->index(index.row(), 0, index.parent()), index);
    }

    // a recursively filtering proxy follows all of these changes
    void testFilteredProxy()
    {
        auto root = std::make_unique<QObject>();
        auto other = std::make_unique<QObject>();
        auto container = new QObject(root.get());
        auto match = new QObject(container);
        match->setObjectName(QStringLiteral("match"));
        processQueue();
        QVERIFY(filteredProxyIsCorrect());

        // the only match moves away, then back, then a second one appears and the first goes
        match->setParent(other.get());
        processQueue();
        QVERIFY(filteredProxyIsCorrect());
        match->setParent(container);
        processQueue();
        QVERIFY(filteredProxyIsCorrect());
        auto match2 = new QObject(root.get());
        match2->setObjectName(QStringLiteral("match2"));
        processQueue();
        QVERIFY(filteredProxyIsCorrect());
        delete match;
        processQueue();
        QVERIFY(filteredProxyIsCorrect());
        delete container;
        processQueue();
        QVERIFY(filteredProxyIsCorrect());
    }

private:
    std::unique_ptr<QAbstractItemModelTester> m_listTester;
    std::unique_ptr<QAbstractItemModelTester> m_treeTester;
    std::unique_ptr<QSortFilterProxyModel> m_filteredProxy;
    std::unique_ptr<QAbstractItemModelTester> m_proxyTester;
};

QTEST_GUILESS_MAIN(ObjectModelTest)

#include "objectmodeltest.moc"
