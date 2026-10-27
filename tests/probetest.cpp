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
