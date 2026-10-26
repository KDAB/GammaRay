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
#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include <algorithm>

using namespace GammaRay;

namespace {
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
