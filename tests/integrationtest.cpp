/*
  integrationtest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "baseprobetest.h"

#include <common/objectmodel.h>
#include <core/probe.h>
#include <core/probeguard.h>

#include <QCoreApplication>
#include <QMetaObject>
#include <QMutexLocker>
#include <QScopeGuard>
#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include <private/qhooks_p.h>

#include <algorithm>
#include <atomic>
#include <memory>
#include <tuple>

using namespace GammaRay;

namespace {
// a signal and a slot, the slot callbacks are only called for connections by method index
class SlotObject : public QObject
{
    Q_OBJECT
public:
    void emitSignal()
    {
        emit fired();
    }

signals:
    void fired();

public slots:
    void slot()
    {
    }
};

// a hook that was installed before GammaRay's
static int s_previousHookCalls = 0;
static void previousAddObjectHook(QObject *)
{
    ++s_previousHookCalls;
}

int argc = 1;
char argv0[] = "integrationtest";
char *argv[] = { argv0 };
}

class IntegrationTest : public BaseProbeTest
{
    Q_OBJECT

private slots:
    void runQCoreApplication()
    {
        {
            QCoreApplication app(argc, argv);
            createProbe();

            QMetaObject::invokeMethod(&app, "quit", Qt::QueuedConnection);
            app.exec();
        }
        QVERIFY(!Probe::instance());
    }

    void runQCoreApplicationWithoutExec()
    {
        {
            QCoreApplication app(argc, argv);
            createProbe();
        }
        QVERIFY(!Probe::instance());
    }

    // announcing a parent first runs slots that can discover the child already (like the widget
    // inspector does for the model of a view), which must not announce the child twice
    void discoverObjectFromObjectCreated()
    {
        {
            QCoreApplication app(argc, argv);
            createProbe();

            QObject *parent = nullptr;
            QObject *child = nullptr;
            {
                ProbeGuard guard; // not known to the probe yet
                parent = new QObject;
                child = new QObject(parent);
            }
            QVERIFY(!Probe::instance()->isValidObject(parent));
            QVERIFY(!Probe::instance()->isValidObject(child));

            connect(Probe::instance(), &Probe::objectCreated, this, [=](QObject *obj) {
                if (obj == parent)
                    Probe::instance()->discoverObject(child);
            });
            QSignalSpy createdSpy(Probe::instance(), &Probe::objectCreated);

            Probe::instance()->discoverObject(child);

            const auto timesCreated = std::count_if(createdSpy.cbegin(), createdSpy.cend(), [child](const QList<QVariant> &args) {
                return args.at(0).value<QObject *>() == child;
            });
            QCOMPARE(timesCreated, 1);
            auto listModel = Probe::instance()->objectListModel();
            QCOMPARE(listModel->match(listModel->index(0, 0), ObjectModel::ObjectRole, QVariant::fromValue(child), -1, Qt::MatchExactly).size(), 1);

            delete parent;
        }
        QVERIFY(!Probe::instance());
    }

    // reparenting a known object to a parent the probe doesn't know yet has to add the parent
    // first, the object tree can't move a row to a parent it doesn't have
    void reparentToUntrackedParent()
    {
        {
            QCoreApplication app(argc, argv);
            createProbe();
            auto treeModel = Probe::instance()->objectTreeModel();
            const auto find = [treeModel](QObject *obj) {
                return treeModel->match(treeModel->index(0, 0), ObjectModel::ObjectRole, QVariant::fromValue(obj), 1,
                                        Qt::MatchExactly | Qt::MatchRecursive)
                    .value(0);
            };

            QObject obj;
            QTRY_VERIFY(find(&obj).isValid()); // also waits for the probe's event filter

            QObject *newParent = nullptr;
            {
                ProbeGuard guard; // not known to the probe yet
                newParent = new QObject;
            }
            QVERIFY(!Probe::instance()->isValidObject(newParent));

            obj.setParent(newParent);
            QVERIFY(Probe::instance()->isValidObject(newParent));

            QVERIFY(find(newParent).isValid());
            // the move itself is processed delayed
            QTRY_COMPARE(find(&obj).parent(), find(newParent));
            QCOMPARE(treeModel->rowCount(find(newParent)), 1);

            obj.setParent(nullptr);
            delete newParent;
        }
        QVERIFY(!Probe::instance());
    }

    // handlers of a delayed reparent can queue further reparents, which must not get lost
    // the slot end callback of another thread waits for the lock while the probe is destroyed
    void slotEndCallbackDuringProbeDestruction()
    {
        {
            QCoreApplication app(argc, argv);
            createProbe();
            SignalSpyCallbackSet callbacks;
            callbacks.slotEndCallback = [](QObject *, int) {};
            Probe::instance()->registerSignalSpyCallbackSet(callbacks);

            // creating objects needs the lock too, so that happens before the main thread has it
            std::atomic<bool> ready = false;
            std::atomic<bool> go = false;
            std::unique_ptr<QThread> thread(QThread::create([&ready, &go] {
                SlotObject obj;
                QObject::connect(&obj, SIGNAL(fired()), &obj, SLOT(slot())); // not a functor connection
                ready = true;
                while (!go)
                    QThread::yieldCurrentThread();
                obj.emitSignal(); // the callback after the slot waits for the lock
            }));
            thread->start();
            while (!ready)
                QThread::yieldCurrentThread();
            {
                QMutexLocker lock(Probe::objectLock());
                go = true;
                QThread::msleep(100); // let it reach the lock
                delete Probe::instance();
            }
            QVERIFY(thread->wait(10000));
        }
        QVERIFY(!Probe::instance());
    }

    // detaching deletes the probe, attaching again has to track objects again, and in between the
    // hooks GammaRay replaced are back
    void reattach()
    {
        {
            QCoreApplication app(argc, argv);
            // as if this was installed before GammaRay's hooks, which are installed already
            const auto originalHooks = std::make_tuple(qtHookData[QHooks::AddQObject], qtHookData[QHooks::RemoveQObject],
                                                       qtHookData[QHooks::Startup]);
            const auto restoreHooks = qScopeGuard([originalHooks] {
                std::tie(qtHookData[QHooks::AddQObject], qtHookData[QHooks::RemoveQObject], qtHookData[QHooks::Startup]) = originalHooks;
            });
            qtHookData[QHooks::AddQObject] = reinterpret_cast<quintptr>(&previousAddObjectHook);
            qtHookData[QHooks::RemoveQObject] = 0;
            qtHookData[QHooks::Startup] = 0;
            QVERIFY(!Hooks::hooksInstalled());

            createProbe();
            QVERIFY(Hooks::hooksInstalled());
            s_previousHookCalls = 0;
            delete new QObject;
            QCOMPARE(s_previousHookCalls, 1); // GammaRay calls the hook it replaced

            delete Probe::instance();
            QCOMPARE(qtHookData[QHooks::AddQObject], reinterpret_cast<quintptr>(&previousAddObjectHook));

            gammaray_probe_attach();
            QTRY_VERIFY(Probe::instance());
            QVERIFY(Hooks::hooksInstalled());
            auto obj = new QObject;
            QTRY_VERIFY(Probe::instance()->isValidObject(obj));
            delete obj;
            QVERIFY(!Probe::instance()->isValidObject(obj));
        }
        QVERIFY(!Probe::instance());
    }

    void reparentFromDelayedReparent()
    {
        {
            QCoreApplication app(argc, argv);
            createProbe();
            auto treeModel = Probe::instance()->objectTreeModel();
            const auto find = [treeModel](QObject *obj) {
                return treeModel->match(treeModel->index(0, 0), ObjectModel::ObjectRole, QVariant::fromValue(obj), 1,
                                        Qt::MatchExactly | Qt::MatchRecursive)
                    .value(0);
            };

            QObject obj1;
            QObject obj2;
            QTRY_VERIFY(find(&obj1).isValid() && find(&obj2).isValid());

            QObject *parent1 = nullptr;
            QObject *parent2 = nullptr;
            {
                ProbeGuard guard; // not known to the probe yet, so the reparents are delayed
                parent1 = new QObject;
                parent2 = new QObject;
            }

            bool reparented = false;
            connect(Probe::instance(), &Probe::objectReparented, this, [&](QObject *obj) {
                if (obj == &obj1 && !reparented) {
                    reparented = true;
                    obj2.setParent(parent2);
                }
            });

            obj1.setParent(parent1);
            QTRY_VERIFY(reparented);
            QTRY_COMPARE(find(&obj2).parent(), find(parent2));
            QCOMPARE(find(&obj1).parent(), find(parent1));

            obj1.setParent(nullptr);
            obj2.setParent(nullptr);
            delete parent1;
            delete parent2;
        }
        QVERIFY(!Probe::instance());
    }
};

QTEST_APPLESS_MAIN(IntegrationTest)

#include "integrationtest.moc"
