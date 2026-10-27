/*
  probetest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2026 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "baseprobetest.h"

#include <common/objectmodel.h>
#include <core/probe.h>
#include <core/probeguard.h>

#include <QAbstractItemModel>
#include <QHash>
#include <QSet>
#include <QSignalSpy>

#include <memory>
#include <vector>

using namespace GammaRay;

namespace GammaRay {
// hidden from the object models, because of its namespace
class FilteredObject : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
};
}

class DerivedObject : public QObject
{
    Q_OBJECT
public:
    using QObject::QObject;
};

// lets the event loop run, so the probe processes the object changes it queued
static void processQueue()
{
    for (int i = 0; i < 3; ++i)
        QTest::qWait(1);
}

// checks the signals of the probe against each other: every object is announced once, and before
// it is reparented. Objects destroyed before the probe processed their creation are not announced,
// but their destruction is, and a child can be announced before its parent, when it got that
// parent before the probe processed the creation of either.
class ProbeObserver : public QObject
{
    Q_OBJECT
public:
    explicit ProbeObserver(QAbstractItemModel *listModel)
    {
        for (int row = 0; row < listModel->rowCount(); ++row)
            m_announced.insert(listModel->index(row, 0).data(ObjectModel::ObjectRole).value<QObject *>());

        connect(Probe::instance(), &Probe::objectCreated, this, [this](QObject *obj) {
            if (m_announced.contains(obj))
                errors.push_back(QStringLiteral("announced twice: %1").arg(name(obj)));
            m_announced.insert(obj);
        });
        connect(Probe::instance(), &Probe::objectDestroyed, this, [this](QObject *obj) {
            m_announced.remove(obj);
        });
        connect(Probe::instance(), &Probe::objectReparented, this, [this](QObject *obj) {
            if (!m_announced.contains(obj))
                errors.push_back(QStringLiteral("reparented without being announced: %1").arg(name(obj)));
            else if (obj->parent() && !m_announced.contains(obj->parent()))
                errors.push_back(QStringLiteral("reparented to an object that wasn't announced: %1").arg(name(obj)));
        });
    }

    QStringList errors;

private:
    static QString name(QObject *obj)
    {
        return obj->objectName().isEmpty() ? QString::fromLatin1(obj->metaObject()->className()) : obj->objectName();
    }

    QSet<QObject *> m_announced;
};

// the objects a test created, as far as they are still alive
class ObjectPool : public QObject
{
    Q_OBJECT
public:
    QObject *create(QObject *parent = nullptr)
    {
        auto obj = new QObject(parent);
        obj->setObjectName(QStringLiteral("pool%1").arg(m_counter++));
        alive.insert(obj);
        connect(obj, &QObject::destroyed, this, [this](QObject *o) { alive.remove(o); }, Qt::DirectConnection);
        return obj;
    }

    QSet<QObject *> alive;

private:
    int m_counter = 0;
};

class ProbeTest : public BaseProbeTest
{
    Q_OBJECT
private:
    // the objects the model shows, and their parents
    static QHash<QObject *, QObject *> treeContents(QAbstractItemModel *model, QStringList &errors,
                                                    const QModelIndex &parent = {}, QObject *parentObject = nullptr)
    {
        QHash<QObject *, QObject *> contents;
        for (int row = 0; row < model->rowCount(parent); ++row) {
            const auto index = model->index(row, 0, parent);
            auto obj = index.data(ObjectModel::ObjectRole).value<QObject *>();
            if (contents.contains(obj))
                errors.push_back(QStringLiteral("listed twice in the tree"));
            contents.insert(obj, parentObject);
            const auto children = treeContents(model, errors, index, obj);
            for (auto it = children.cbegin(); it != children.cend(); ++it) {
                if (contents.contains(it.key()))
                    errors.push_back(QStringLiteral("listed twice in the tree"));
                contents.insert(it.key(), it.value());
            }
        }
        return contents;
    }

    static QSet<QObject *> listContents(QAbstractItemModel *model, QStringList &errors)
    {
        QSet<QObject *> contents;
        for (int row = 0; row < model->rowCount(); ++row) {
            auto obj = model->index(row, 0).data(ObjectModel::ObjectRole).value<QObject *>();
            if (contents.contains(obj))
                errors.push_back(QStringLiteral("listed twice in the list"));
            contents.insert(obj);
        }
        return contents;
    }

    // the probe and its models know exactly the alive objects of the pool, at their positions
    static QStringList checkModels(const ObjectPool &pool, const QSet<QObject *> &everCreated)
    {
        QStringList errors;
        const auto tree = treeContents(Probe::instance()->objectTreeModel(), errors);
        const auto list = listContents(Probe::instance()->objectListModel(), errors);
        for (auto obj : pool.alive) {
            if (!Probe::instance()->isValidObject(obj))
                errors.push_back(QStringLiteral("not valid: %1").arg(obj->objectName()));
            if (!list.contains(obj))
                errors.push_back(QStringLiteral("not in the list: %1").arg(obj->objectName()));
            if (!tree.contains(obj))
                errors.push_back(QStringLiteral("not in the tree: %1").arg(obj->objectName()));
            else if (tree.value(obj) != obj->parent())
                errors.push_back(QStringLiteral("wrong parent in the tree: %1").arg(obj->objectName()));
        }
        for (auto obj : everCreated) {
            if (pool.alive.contains(obj))
                continue;
            if (list.contains(obj) || tree.contains(obj))
                errors.push_back(QStringLiteral("deleted object still listed"));
        }
        return errors;
    }

private slots:
    void initTestCase()
    {
        createProbe();
    }

    // QTest::qExec() doesn't emit aboutToQuit(), so the probe would only go away with the statics
    // it depends on
    static void cleanupTestCase()
    {
        delete Probe::instance();
    }

    // a known object without a parent gets one that the probe doesn't know about yet, so there is
    // no ChildRemoved event for it, and its move can only be reported after the parent's creation
    static void testReparentToObjectNotAnnouncedYet()
    {
        ObjectPool pool;
        auto obj = pool.create();
        processQueue();
        QVERIFY(Probe::instance()->isValidObject(obj));

        auto parent = pool.create();
        obj->setParent(parent);
        processQueue();
        const auto errors = checkModels(pool, { obj, parent });
        QVERIFY2(errors.isEmpty(), qPrintable(errors.join(QLatin1Char('\n'))));
        delete parent;
    }

    // a new parent that was below the moved object, and moved away before, but the probe reports
    // that move only after this one
    static void testReparentBelowFormerDescendant()
    {
        ObjectPool pool;
        auto a = pool.create();
        auto b = pool.create(a);
        processQueue();

        b->setParent(nullptr); // reported later
        a->setParent(b); // reported right away
        processQueue();
        const auto errors = checkModels(pool, { a, b });
        QVERIFY2(errors.isEmpty(), qPrintable(errors.join(QLatin1Char('\n'))));
        delete b;
    }

    // the move of a child is reported later than the destruction of the parent it left
    static void testParentDeletedBeforeChildMoveReported()
    {
        ObjectPool pool;
        auto parent = pool.create();
        auto child = pool.create(parent);
        pool.create(child);
        processQueue();

        child->setParent(nullptr); // reported later
        delete parent; // reported right away
        processQueue();
        const auto errors = checkModels(pool, { parent, child });
        QVERIFY2(errors.isEmpty(), qPrintable(errors.join(QLatin1Char('\n'))));
        delete child;
    }

    // a child announced before its parent makes the tree model add the parent early, which can
    // still move before it is announced itself
    static void testParentMovedBeforeItsAnnouncement()
    {
        ObjectPool pool;
        auto newParent = pool.create();
        processQueue();
        auto child = pool.create();
        auto parent = pool.create();
        child->setParent(parent); // announced first
        auto connection = connect(Probe::instance(), &Probe::objectCreated, &pool, [&](QObject *o) {
            if (o == child)
                parent->setParent(newParent);
        });
        processQueue();
        disconnect(connection);
        const auto errors = checkModels(pool, { newParent, child, parent });
        QVERIFY2(errors.isEmpty(), qPrintable(errors.join(QLatin1Char('\n'))));
        delete newParent;
    }

    // every move to a parent without children adds an entry to the tree model's map, at some point
    // that makes it grow while the model still refers to the entry of the old parent
    static void testReparentToParentsWithoutChildren()
    {
        ObjectPool pool;
        QSet<QObject *> everCreated;
        std::vector<QObject *> objects;
        for (int i = 0; i < 500; ++i) {
            objects.push_back(pool.create(pool.create()));
            everCreated.insert(objects.back());
            everCreated.insert(objects.back()->parent());
        }
        processQueue();
        std::vector<QObject *> newParents;
        for (int i = 0; i < 500; ++i) {
            newParents.push_back(pool.create());
            everCreated.insert(newParents.back());
        }
        processQueue();
        for (int i = 0; i < 500; ++i)
            objects[i]->setParent(newParents[i]);
        processQueue();
        const auto errors = checkModels(pool, everCreated);
        QVERIFY2(errors.isEmpty(), qPrintable(errors.mid(0, 10).join(QLatin1Char('\n'))));
        for (auto obj : std::as_const(pool.alive)) {
            if (!obj->parent())
                obj->deleteLater();
        }
        processQueue();
    }

    // objectCreated() is only emitted once the object is fully constructed
    static void testAnnouncedFullyConstructed()
    {
        const QMetaObject *seen = nullptr;
        DerivedObject *obj = nullptr;
        auto connection = connect(Probe::instance(), &Probe::objectCreated, Probe::instance(), [&](QObject *o) {
            if (o == obj)
                seen = o->metaObject();
        });
        obj = new DerivedObject;
        processQueue();
        disconnect(connection);
        QCOMPARE(seen, &DerivedObject::staticMetaObject);
        delete obj;
    }

    // an object that is gone before the probe processed its creation is never announced (its
    // destruction is, the models just ignore that)
    static void testShortLivedObjectsNotAnnounced()
    {
        int created = 0;
        QObject *obj = nullptr;
        auto connection = connect(Probe::instance(), &Probe::objectCreated, Probe::instance(), [&](QObject *o) {
            created += o == obj;
        });
        obj = new QObject;
        delete obj;
        processQueue();
        disconnect(connection);
        QCOMPARE(created, 0);
    }

    // slots connected to objectCreated() can create further objects, those are announced too
    static void testCreateFromObjectCreated()
    {
        auto trigger = std::make_unique<QObject>();
        trigger->setObjectName(QStringLiteral("trigger"));
        std::unique_ptr<QObject> created;
        auto connection = connect(Probe::instance(), &Probe::objectCreated, Probe::instance(), [&](QObject *o) {
            if (o == trigger.get())
                created = std::make_unique<QObject>();
        });
        QTRY_VERIFY(created);
        disconnect(connection);
        processQueue();
        QVERIFY(Probe::instance()->isValidObject(created.get()));
        ObjectPool pool;
        pool.alive.insert(created.get());
        const auto errors = checkModels(pool, {});
        QVERIFY2(errors.isEmpty(), qPrintable(errors.join(QLatin1Char('\n'))));
    }

    // slots connected to objectCreated() can delete the object right away
    static void testDeleteFromObjectCreated()
    {
        ObjectPool pool;
        ProbeObserver observer(Probe::instance()->objectListModel());
        QObject *parent = pool.create();
        QObject *child = pool.create(parent);
        QSet<QObject *> everCreated { parent, child };
        auto connection = connect(Probe::instance(), &Probe::objectCreated, Probe::instance(), [&](QObject *o) {
            if (o == child)
                delete child;
        });
        processQueue();
        disconnect(connection);
        QVERIFY(!pool.alive.contains(child));
        auto errors = checkModels(pool, everCreated) + observer.errors;
        QVERIFY2(errors.isEmpty(), qPrintable(errors.join(QLatin1Char('\n'))));
        delete parent;
        processQueue();
        errors = checkModels(pool, everCreated) + observer.errors;
        QVERIFY2(errors.isEmpty(), qPrintable(errors.join(QLatin1Char('\n'))));
    }

    // GammaRay's own objects are hidden, together with everything below them
    static void testFilteredObjects()
    {
        auto filtered = new FilteredObject;
        auto belowFiltered = new QObject(filtered);
        auto probeChild = new QObject(Probe::instance());
        ObjectPool pool;
        auto normal = pool.create();
        processQueue();
        QVERIFY(!Probe::instance()->isValidObject(filtered));
        QVERIFY(!Probe::instance()->isValidObject(belowFiltered));
        QVERIFY(!Probe::instance()->isValidObject(probeChild));
        QVERIFY(Probe::instance()->isValidObject(normal));

        QStringList errors;
        const auto list = listContents(Probe::instance()->objectListModel(), errors);
        QVERIFY(!list.contains(filtered));
        QVERIFY(!list.contains(belowFiltered));
        QVERIFY(!list.contains(probeChild));
        QVERIFY(list.contains(normal));

        delete normal;
        delete probeChild;
        delete filtered;
    }

    // objects created by GammaRay itself are ignored
    static void testProbeGuard()
    {
        QObject *obj = nullptr;
        {
            ProbeGuard guard;
            obj = new QObject;
        }
        processQueue();
        QVERIFY(!Probe::instance()->isValidObject(obj));
        delete obj;
    }

    static void testFavorites()
    {
        auto obj = std::make_unique<QObject>();
        processQueue();
        QSignalSpy favoritedSpy(Probe::instance(), &Probe::objectFavorited);
        QSignalSpy unfavoritedSpy(Probe::instance(), &Probe::objectUnfavorited);
        Probe::instance()->markObjectAsFavorite(obj.get());
        QCOMPARE(favoritedSpy.size(), 1);
        QCOMPARE(favoritedSpy.at(0).at(0).value<QObject *>(), obj.get());
        Probe::instance()->removeObjectAsFavorite(obj.get());
        QCOMPARE(unfavoritedSpy.size(), 1);
        QCOMPARE(unfavoritedSpy.at(0).at(0).value<QObject *>(), obj.get());
    }

    // only types that a tool can show are selected
    static void testSelectNonQObject()
    {
        QSignalSpy spy(Probe::instance(), &Probe::nonQObjectSelected);
        auto metaObject = const_cast<QMetaObject *>(&QObject::staticMetaObject);
        Probe::instance()->selectObject(metaObject, QStringLiteral("QMetaObject"));
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(0).value<void *>(), static_cast<void *>(metaObject));
        QCOMPARE(spy.at(0).at(1).toString(), QStringLiteral("QMetaObject"));

        int value = 42;
        Probe::instance()->selectObject(&value, QStringLiteral("int"));
        QCOMPARE(spy.size(), 1);
    }
};

QTEST_GUILESS_MAIN(ProbeTest)

#include "probetest.moc"
