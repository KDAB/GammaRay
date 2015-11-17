/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include <config-gammaray.h>

#include <plugins/quickinspector/quickinspectorinterface.h>
#include <probe/hooks.h>
#include <probe/probecreator.h>
#include <core/probe.h>
#include <common/paths.h>
#include <common/objectbroker.h>

#include <3rdparty/qt/modeltest.h>

#include <QtTest/qtest.h>

#include <QQuickView>
#include <QItemSelectionModel>
#include <QRegExp>
#include <QSignalSpy>

#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
Q_DECLARE_METATYPE(QItemSelection)
#endif

using namespace GammaRay;

class QuickInspectorTest : public QObject
{
    Q_OBJECT
private:
    void createProbe()
    {
        Paths::setRelativeRootPath(GAMMARAY_INVERSE_BIN_DIR);
        qputenv("GAMMARAY_ProbePath", Paths::currentProbePath().toUtf8());
        Hooks::installHooks();
        Probe::startupHookReceived();
        new ProbeCreator(ProbeCreator::CreateOnly);
        QTest::qWait(1); // event loop re-entry
    }

private slots:
    void initTestCase()
    {
        qRegisterMetaType<QItemSelection>();
    }
    void testModelsReparent()
    {
        createProbe();

        // we need one view for the plugin to activate, otherwise the model will not be available
        auto view = new QQuickView;
        view->show();
        QTest::qWait(1); // event loop re-entry

        auto windowModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickWindowModel"));
        QVERIFY(windowModel);
        ModelTest windowModelTest(windowModel);
        QCOMPARE(windowModel->rowCount(), 1);

        auto itemModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickItemModel"));
        QVERIFY(itemModel);
        ModelTest itemModelTest(itemModel);

        auto sgModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickSceneGraphModel"));
        QVERIFY(sgModel);
        ModelTest sgModelTest(sgModel);

        auto inspector = ObjectBroker::object<QuickInspectorInterface*>();
        QVERIFY(inspector);
        inspector->selectWindow(0);
        QTest::qWait(1);

        view->setSource(QUrl(QStringLiteral("qrc:/manual/reparenttest.qml")));
        QTest::qWait(20); // wait at least one frame

        QTest::keyClick(view, Qt::Key_Right);
        QTest::qWait(20);
        QTest::keyClick(view, Qt::Key_Left);
        QTest::qWait(20);
        QTest::keyClick(view, Qt::Key_Right);
        QTest::qWait(20);

        delete view;
        QTest::qWait(1);
    }

    void testModelsCreateDestroy()
    {
        createProbe();

        // we need one view for the plugin to activate, otherwise the model will not be available
        auto view = new QQuickView;
        view->show();
        QTest::qWait(1); // event loop re-entry

        auto windowModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickWindowModel"));
        QVERIFY(windowModel);
        ModelTest windowModelTest(windowModel);
        QCOMPARE(windowModel->rowCount(), 1);

        auto itemModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickItemModel"));
        QVERIFY(itemModel);
        ModelTest itemModelTest(itemModel);

        auto sgModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickSceneGraphModel"));
        QVERIFY(sgModel);
        ModelTest sgModelTest(sgModel);

        auto inspector = ObjectBroker::object<QuickInspectorInterface*>();
        QVERIFY(inspector);
        inspector->selectWindow(0);
        QTest::qWait(1);

        view->setSource(QUrl(QStringLiteral("qrc:/manual/quickitemcreatedestroytest.qml")));
        QTest::qWait(20); // wait at least one frame

        // scroll through the list, to trigger creations/destructions
        for (int i = 0; i < 30; ++i)
            QTest::keyClick(view, Qt::Key_Down);
        QTest::qWait(20);
        for (int i = 0; i < 30; ++i)
            QTest::keyClick(view, Qt::Key_Up);
        QTest::qWait(20);

        delete view;
        QTest::qWait(1);
    }

    void testModelsCreateDestroyProxy()
    {
        createProbe();

        // we need one view for the plugin to activate, otherwise the model will not be available
        auto view = new QQuickView;
        view->show();
        QTest::qWait(1); // event loop re-entry

        auto itemModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickItemModel"));
        QVERIFY(itemModel);
        ModelTest itemModelTest(itemModel);

        auto sgModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickSceneGraphModel"));
        QVERIFY(sgModel);
        ModelTest sgModelTest(sgModel);

        auto inspector = ObjectBroker::object<QuickInspectorInterface*>();
        QVERIFY(inspector);
        inspector->selectWindow(0);
        QTest::qWait(1);

        view->setSource(QUrl(QStringLiteral("qrc:/manual/quickitemcreatedestroytest.qml")));
        QTest::qWait(20); // wait at least one frame
        QVERIFY(itemModel->rowCount() > 0);
        QVERIFY(sgModel->rowCount() > 0);

        itemModel->setProperty("filterKeyColumn", -1);
        itemModel->setProperty("filterRegExp", QRegExp("Rect", Qt::CaseInsensitive, QRegExp::FixedString));
        sgModel->setProperty("filterKeyColumn", -1);
        sgModel->setProperty("filterRegExp", QRegExp("Transform", Qt::CaseInsensitive, QRegExp::FixedString));
        QVERIFY(itemModel->rowCount() > 0);
        QVERIFY(sgModel->rowCount() > 0);

        // scroll through the list, to trigger creations/destructions
        for (int i = 0; i < 30; ++i)
            QTest::keyClick(view, Qt::Key_Down);
        QTest::qWait(20);

        itemModel->setProperty("filterRegExp", QRegExp());
        sgModel->setProperty("filterRegExp", QRegExp());
        QTest::qWait(20);

        delete view;
        QTest::qWait(1);
    }

    void testItemPicking()
    {
        createProbe();

        // we need one view for the plugin to activate, otherwise the model will not be available
        auto view = new QQuickView;
        view->show();
        QTest::qWait(1); // event loop re-entry

        auto toolModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ToolModel"));
        QVERIFY(toolModel);

        auto itemModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickItemModel"));
        QVERIFY(itemModel);

        auto sgModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickSceneGraphModel"));
        QVERIFY(sgModel);

        auto inspector = ObjectBroker::object<QuickInspectorInterface*>();
        QVERIFY(inspector);
        inspector->selectWindow(0);
        QTest::qWait(1);

        view->setSource(QUrl(QStringLiteral("qrc:/manual/reparenttest.qml")));
        QTest::qWait(20); // wait at least one frame

        auto toolSelectionModel = ObjectBroker::selectionModel(toolModel);
        QVERIFY(toolSelectionModel);
        QSignalSpy toolSpy(toolSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)));
        QVERIFY(toolSpy.isValid());

        auto itemSelectionModel = ObjectBroker::selectionModel(itemModel);
        QVERIFY(itemSelectionModel);
        QSignalSpy itemSpy(itemSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)));
        QVERIFY(itemSpy.isValid());

        auto sgSelectionModel = ObjectBroker::selectionModel(sgModel);
        QVERIFY(sgModel);
        QSignalSpy sgSpy(sgSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)));
        QVERIFY(sgSpy.isValid());

        // auto center-click is broken before https://codereview.qt-project.org/141085/
        QTest::mouseClick(view, Qt::LeftButton, Qt::ShiftModifier | Qt::ControlModifier, QPoint(view->width()/2, view->height()/2));
        QTest::qWait(20);

        QCOMPARE(toolSpy.size(), 1);
        QCOMPARE(itemSpy.size(), 1);
        QCOMPARE(sgSpy.size(), 1);

        delete view;
        QTest::qWait(1);
    }
};

QTEST_MAIN(QuickInspectorTest)

#include "quickinspectortest.moc"
