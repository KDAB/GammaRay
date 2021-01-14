/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2018-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "baseprobetest.h"

#include <core/problemcollector.h>
#include <common/problem.h>
#include <common/sourcelocation.h>
#include <common/objectbroker.h>
#include <3rdparty/qt/modeltest.h>

#include <common/tools/problemreporter/problemmodelroles.h>

#include <QDebug>
#include <QTest>
#include <QObject>
#include <QThread>
#include <QThreadPool>
#include <QRunnable>
#include <QUuid>

#ifdef QT_QML_LIB
#include <QQmlEngine>
#include <QQmlComponent>
#include <QQmlContext>
#endif

#ifdef HAVE_QT_WIDGETS
#include <QAction>
#include <QKeySequence>
#endif

using namespace GammaRay;

class CrossThreadConnectionTask : public QRunnable
{
public:
    void run() override
    {
        newThreadObj.reset(new QObject());
        newThreadObj->setObjectName("newThreadObj");
        QObject::connect(newThreadObj.get(), &QObject::destroyed, mainThreadObj.get(), &QObject::deleteLater, Qt::DirectConnection);
    }
    std::unique_ptr<QObject> newThreadObj;
    std::unique_ptr<QObject> mainThreadObj;
};

struct UnregisteredType {};

class FaultyMetaObjectBaseClass : public QObject
{
    Q_OBJECT
    Q_PROPERTY(UnregisteredType someProp READ someProp CONSTANT)
    UnregisteredType someProp() const { return {}; }
};

class FaultyMetaObjectClass : public FaultyMetaObjectBaseClass
{
    Q_OBJECT

    Q_PROPERTY(UnregisteredType someProp READ someProp CONSTANT)

public:
    Q_INVOKABLE void noop(UnregisteredType param) { Q_UNUSED(param) }

    UnregisteredType someProp() const { return {}; }
};

namespace GammaRay {
class ProblemReporterTest : public BaseProbeTest
{
    Q_OBJECT

    static void dummyScan()
    {
        Problem p1;
        p1.problemId = QUuid::createUuid().toString();
        p1.findingCategory = Problem::Scan;
        ProblemCollector::addProblem(p1);

        Problem p2;
        p2.problemId = QUuid::createUuid().toString();
        p2.findingCategory = Problem::Scan;
        ProblemCollector::addProblem(p2);
    }

    std::unique_ptr<ModelTest> problemModelTest;
    std::unique_ptr<ModelTest> availableCheckersModelTest;

private slots:
    void initTestCase()
    {
        createProbe();
        problemModelTest.reset(new ModelTest(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ProblemModel"))));
        availableCheckersModelTest.reset(new ModelTest(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.AvailableProblemCheckersModel"))));
    }

    void cleanup()
    {
        ProblemCollector::instance()->clearScans();
        QCOMPARE(ProblemCollector::instance()->problems().size(), 0);
    }

    void testDuplicates()
    {
        QCOMPARE(ProblemCollector::instance()->problems().size(), 0);

        Problem p1;
        p1.problemId = QStringLiteral("9skjlksdjb");
        ProblemCollector::addProblem(p1);

        Problem p2;
        p2.problemId = QStringLiteral("9skjlksdjb");
        ProblemCollector::addProblem(p2);

        QCOMPARE(ProblemCollector::instance()->problems().size(), 1);

        ProblemCollector::removeProblem(QStringLiteral("9skjlksdjb"));
    }

    void testMultipleSourceLocations()
    {
        QCOMPARE(ProblemCollector::instance()->problems().size(), 0);

        SourceLocation l1 = SourceLocation::fromOneBased(QUrl("../main.qml"), 101, 42);
        SourceLocation l2 = SourceLocation::fromOneBased(QUrl("A.qml"), 43, 21);
        SourceLocation l3 = SourceLocation::fromOneBased(QUrl("../../src/Singleton.cpp"), 555, 1);
        SourceLocation l4 = SourceLocation::fromOneBased(QUrl("../main.qml"), 101, 42);
        SourceLocation l5 = SourceLocation::fromOneBased(QUrl("A.qml"), 44, 21);

        Problem p1;
        p1.problemId = QStringLiteral("abcdefg");
        ProblemCollector::addProblem(p1);

        Problem p2;
        p2.problemId = QStringLiteral("abcdefg");
        p2.locations << l1;
        ProblemCollector::addProblem(p2);

        Problem p3;
        p3.problemId = QStringLiteral("abcdefg");
        p3.locations << l2 << l3 << l4;
        ProblemCollector::addProblem(p3);

        Problem p4;
        p4.problemId = QStringLiteral("abcdefg");
        p4.locations << l5;
        ProblemCollector::addProblem(p4);
        QCOMPARE(ProblemCollector::instance()->problems().size(), 1);

        QCOMPARE(ProblemCollector::instance()->problems().front().locations.size(), 4);
        QCOMPARE(ProblemCollector::instance()->problems().front().locations.at(0), SourceLocation::fromOneBased(QUrl("../main.qml"), 101, 42));
        QCOMPARE(ProblemCollector::instance()->problems().front().locations.at(1), SourceLocation::fromOneBased(QUrl("A.qml"), 43, 21));
        QCOMPARE(ProblemCollector::instance()->problems().front().locations.at(2), SourceLocation::fromOneBased(QUrl("../../src/Singleton.cpp"), 555, 1));
        QCOMPARE(ProblemCollector::instance()->problems().front().locations.at(3), SourceLocation::fromOneBased(QUrl("A.qml"), 44, 21));

        ProblemCollector::removeProblem(QStringLiteral("abcdefg"));
    }

    void testScans()
    {
        auto standardCheckersCount = ProblemCollector::instance()->availableCheckers().size();
        ProblemCollector::registerProblemChecker(QStringLiteral("Dummy"),
                                                 QStringLiteral("Dummy"),
                                                 QStringLiteral("Always reports two dummy problems"),
                                                 &ProblemReporterTest::dummyScan);

        QCOMPARE(ProblemCollector::instance()->availableCheckers().size(), standardCheckersCount + 1);

        QCOMPARE(ProblemCollector::instance()->problems().size(), 0);
        ProblemCollector::instance()->requestScan();
        auto problemsFromScansCount = ProblemCollector::instance()->problems().size();
        ProblemCollector::instance()->requestScan(); // scans should always be reproducable if the program didn't change.
        QCOMPARE(ProblemCollector::instance()->problems().size(), problemsFromScansCount);

        auto dummyChecker = std::find_if(ProblemCollector::instance()->availableCheckers().begin(),
                                         ProblemCollector::instance()->availableCheckers().end(),
                                         [](const ProblemCollector::Checker &c) { return c.id == QStringLiteral("Dummy"); }
                                        );
        dummyChecker->enabled = false;

        ProblemCollector::instance()->requestScan(); // scans should always be reproducable if the program didn't change.
        QCOMPARE(ProblemCollector::instance()->problems().size(), problemsFromScansCount - 2);
        dummyChecker->enabled = true;

        Problem p1;
        p1.problemId = QStringLiteral("9skjlksdjb");
        p1.findingCategory = Problem::Live;
        ProblemCollector::addProblem(p1);
        Problem p2;
        p2.problemId = QStringLiteral("abcdefg");
        p2.findingCategory = Problem::Permanent;
        ProblemCollector::addProblem(p2);

        // all problems originating from a scan should be deleted before doing a new scan, but not live- and permanent problems
        ProblemCollector::instance()->requestScan();
        QCOMPARE(ProblemCollector::instance()->problems().size(), problemsFromScansCount + 2);


        ProblemCollector::instance()->clearScans();
        QCOMPARE(ProblemCollector::instance()->problems().size(), 2);
        ProblemCollector::removeProblem(QStringLiteral("9skjlksdjb"));
        ProblemCollector::removeProblem(QStringLiteral("abcdefg"));

        ProblemCollector::instance()->availableCheckers().erase(dummyChecker);
    }

    void testAvailableScansModel()
    {
        auto model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.AvailableProblemCheckersModel"));
        auto rowCount = model->rowCount();
        ProblemCollector::registerProblemChecker(QStringLiteral("Dummy"),
                                                 QStringLiteral("Dummy"),
                                                 QStringLiteral("Always reports two dummy problems"),
                                                 &ProblemReporterTest::dummyScan);
        QCOMPARE(model->rowCount(), rowCount + 1);

        auto &checkers = ProblemCollector::instance()->availableCheckers();
        checkers.erase(std::remove_if(checkers.begin(),
                                      checkers.end(),
                                      [](ProblemCollector::Checker &c) { return c.id == "Dummy"; }
                                     ));
        QCOMPARE(model->rowCount(), rowCount);
    }

    void testProblemModel()
    {
        auto model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ProblemModel"));
        auto rowCount = model->rowCount();

        Problem p1;
        p1.problemId = QStringLiteral("9skjlksdjb");
        p1.description = QStringLiteral("Lorem ipsum dolor sit amet.");
        p1.severity = Problem::Warning;
        SourceLocation l1 = SourceLocation::fromOneBased(QUrl("../main.qml"), 101, 42);
        SourceLocation l2 = SourceLocation::fromOneBased(QUrl("A.qml"), 43, 21);
        p1.object = ObjectId(this);
        p1.locations << l1 << l2;
        ProblemCollector::addProblem(p1);

        Problem p2;
        p2.problemId = QStringLiteral("asdf");
        ProblemCollector::addProblem(p2);

        QCOMPARE(model->rowCount(), rowCount + 2);

        auto index = model->index(rowCount, 0, QModelIndex());
        QCOMPARE(index.data(Qt::DisplayRole).toString(), QStringLiteral("Lorem ipsum dolor sit amet."));
        QCOMPARE(index.sibling(rowCount, 1).data(Qt::DisplayRole).toString(), l1.displayString());
        QCOMPARE(index.data(ObjectModel::ObjectIdRole).value<ObjectId>(), ObjectId(this));
        QCOMPARE(index.data(ProblemModelRoles::SourceLocationRole).value<QVector<SourceLocation>>(), p1.locations);
        QCOMPARE(index.data(ProblemModelRoles::SeverityRole).value<int>(), static_cast<int>(Problem::Warning));
        QCOMPARE(index.data(ProblemModelRoles::ProblemIdRole).toString(), QStringLiteral("9skjlksdjb"));


        ProblemCollector::removeProblem(QStringLiteral("9skjlksdjb"));
        QCOMPARE(model->rowCount(), rowCount + 1);
        ProblemCollector::removeProblem(QStringLiteral("asdf"));
        QCOMPARE(model->rowCount(), rowCount);
    }

#ifdef QT_QML_LIB
    void testBindingLoopChecker()
    {
        QQmlEngine engine;
        QQmlComponent c(&engine);
        c.setData("import QtQml 2.0\n"
                  "QtObject{id: root\n"
                  "  property list<QtObject> children: [\n"
                  "  QtObject {id: a; objectName: b.objectName },\n"
                  "  QtObject {id: b; objectName: a.objectName }\n"
                  "  ]\n"
                  "}",
                  QUrl());

        std::unique_ptr<QObject> obj(c.create());
        QTest::qWait(1);
        QVERIFY(static_cast<bool>(obj));

        QVERIFY(ProblemCollector::instance()->isCheckerRegistered("com.kdab.GammaRay.ObjectInspector.BindingLoopScan"));

        ProblemCollector::instance()->requestScan();

#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
        QEXPECT_FAIL("", "Can't find QML bindings with Qt < 5.10.", Abort);
#endif
        const auto &problems = ProblemCollector::instance()->problems();
        QVERIFY(std::any_of(problems.begin(), problems.end(),
            [](const Problem &p){ return p.problemId.startsWith("com.kdab.GammaRay.ObjectInspector.BindingLoopScan"); }
        ));
    }
#endif

    void testConnectionIssues()
    {
        QVERIFY(ProblemCollector::instance()->isCheckerRegistered("com.kdab.GammaRay.ObjectInspector.ConnectionsCheck"));

        auto task = std::unique_ptr<CrossThreadConnectionTask>(new CrossThreadConnectionTask());
        task->mainThreadObj.reset(new QObject());
        task->mainThreadObj->setObjectName("mainThreadObj");
        task->setAutoDelete(false);
        // QThreadPool takes ownership and deletes 'hello' automatically
        QThreadPool::globalInstance()->start(task.get());

        auto o1 = std::unique_ptr<QObject>(new QObject());
        o1->setObjectName("o1");
        auto o2 = std::unique_ptr<QObject>(new QObject());
        o2->setObjectName("o2");
        connect(o1.get(), SIGNAL(destroyed(QObject*)), o2.get(), SLOT(deleteLater()));
        connect(o1.get(), SIGNAL(destroyed(QObject*)), o2.get(), SLOT(deleteLater()));

        QTest::qWait(10);
        ProblemCollector::instance()->requestScan();

        o1->disconnect();
        task->newThreadObj->disconnect();

        const auto &problems = ProblemCollector::instance()->problems();
        auto crossThreadProblem = std::find_if(problems.begin(), problems.end(),
            [](const Problem &p){ return p.problemId.startsWith("com.kdab.GammaRay.ObjectInspector.ConnectionsCheck.CrossTread"); }
        );

        QVERIFY(crossThreadProblem != problems.end());
        QCOMPARE(crossThreadProblem->object, ObjectId(task->mainThreadObj.get()));
        QVERIFY(crossThreadProblem->description.contains("direct cross-thread connection"));
        QVERIFY(crossThreadProblem->description.contains("signal newThreadObj"));
        QVERIFY(crossThreadProblem->description.contains("slot mainThreadObj"));

        auto duplicateProblem = std::find_if(problems.begin(), problems.end(),
            [](const Problem &p){ return p.problemId.startsWith("com.kdab.GammaRay.ObjectInspector.ConnectionsCheck.Duplicate"); }
        );

        QVERIFY(duplicateProblem != problems.end());
        QCOMPARE(duplicateProblem->object, ObjectId(o2.get()));
        QVERIFY(duplicateProblem->description.contains("multiple times"));
        QVERIFY(duplicateProblem->description.contains("signal o1"));
        QVERIFY(duplicateProblem->description.contains("slot o2"));


        disconnect(o1.get(), nullptr, o2.get(), nullptr);
        connect(o1.get(), &QObject::destroyed, o2.get(), &QObject::deleteLater);
        connect(o1.get(), &QObject::destroyed, o2.get(), &QObject::deleteLater);
        QTest::qWait(10);
        ProblemCollector::instance()->requestScan();

        const auto &problems2 = ProblemCollector::instance()->problems();
        auto duplicateProblem2 = std::find_if(problems2.begin(), problems2.end(),
            [&o2](const Problem &p){
                return p.problemId.startsWith("com.kdab.GammaRay.ObjectInspector.ConnectionsCheck.Duplicate")
                       && p.object == ObjectId(o2.get());
            }
        );

        QEXPECT_FAIL("", "We can't find duplicates with PMF connects, yet.", Abort);
        QVERIFY(duplicateProblem2 != problems2.end());
        QCOMPARE(duplicateProblem2->object, ObjectId(o2.get()));
        QVERIFY(duplicateProblem2->description.contains("multiple times"));
        QVERIFY(duplicateProblem2->description.contains("signal o1"));
        QVERIFY(duplicateProblem2->description.contains("slot o2"));


    }

    void testMetaTypeChecks()
    {
        std::unique_ptr<QObject> obj(new FaultyMetaObjectClass);
        QTest::qWait(1);

        auto &checkers = ProblemCollector::instance()->availableCheckers();
        auto checker = std::find_if(checkers.begin(), checkers.end(),
                    [](ProblemCollector::Checker &c){ return c.id == "com.kdab.GammaRay.MetaObjectBrowser.QMetaObjectValidator"; }
                   );
        QVERIFY(checker != checkers.end());
        checker->enabled = true;

        ProblemCollector::instance()->requestScan();

        const auto &problems = ProblemCollector::instance()->problems();
        QVERIFY(std::any_of(problems.begin(), problems.end(),
            [&obj](const Problem &p){
                return p.problemId.startsWith("com.kdab.GammaRay.MetaObjectBrowser.QMetaObjectValidator")
                       && p.object == ObjectId(const_cast<QMetaObject*>(obj->metaObject()), "const QMetaObject*")
                       && p.description.contains(QLatin1String("overrides base class property"));
            }
        ));

        QVERIFY(std::any_of(problems.begin(), problems.end(),
            [&obj](const Problem &p){
                return p.problemId.startsWith("com.kdab.GammaRay.MetaObjectBrowser.QMetaObjectValidator")
                       && p.object == ObjectId(const_cast<QMetaObject*>(obj->metaObject()), "const QMetaObject*")
                       && p.description.contains(QLatin1String("parameter type not registered"));
            }
        ));
        QVERIFY(std::any_of(problems.begin(), problems.end(),
            [&obj](const Problem &p){
                return p.problemId.startsWith("com.kdab.GammaRay.MetaObjectBrowser.QMetaObjectValidator")
                       && p.object == ObjectId(const_cast<QMetaObject*>(obj->metaObject()), "const QMetaObject*")
                       && p.description.contains(QLatin1String("property with a type not registered"));
            }
        ));

    }

#ifdef HAVE_QT_WIDGETS
    void testActionValidator()
    {
        QAction *a1 = new QAction(QStringLiteral("Action 1"), qApp);
        a1->setShortcut(QKeySequence(QStringLiteral("Ctrl+K")));
        a1->setShortcutContext(Qt::ApplicationShortcut);
        QAction *a2 = new QAction(QStringLiteral("Action 2"), qApp);
        a2->setShortcut(QKeySequence(QStringLiteral("Ctrl+K")));
        a2->setShortcutContext(Qt::WidgetShortcut);
        QTest::qWait(1); // event loop re-entry

        QVERIFY(ProblemCollector::instance()->isCheckerRegistered("gammaray_actioninspector.ShortcutDuplicates"));

        ProblemCollector::instance()->requestScan();

        const auto &problems = ProblemCollector::instance()->problems();
        QVERIFY(std::any_of(problems.begin(), problems.end(),
            [=](const Problem &p){
                return p.problemId.startsWith("gammaray_actioninspector.ShortcutDuplicates")
                       && (p.object == ObjectId(a1) || p.object == ObjectId(a2))
                       && p.description.contains("ambigous")
                       && p.description.contains(QKeySequence(QStringLiteral("Ctrl+K")).toString(QKeySequence::NativeText))
                       && p.problemId.endsWith(QKeySequence(QStringLiteral("Ctrl+K")).toString(QKeySequence::PortableText));
            }
        ));
    }
#endif
};

}

QTEST_MAIN(ProblemReporterTest)

#include "problemreportertest.moc"
