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
    void init()
    {
        createProbe();

        // we need one view for the plugin to activate, otherwise the model will not be available
        view = new QQuickView;
        view->show();
        QTest::qWait(1); // event loop re-entry

        itemModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickItemModel"));
        QVERIFY(itemModel);
        ModelTest itemModelTest(itemModel);

        sgModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickSceneGraphModel"));
        QVERIFY(sgModel);
        ModelTest sgModelTest(sgModel);

        inspector = ObjectBroker::object<QuickInspectorInterface*>();
        QVERIFY(inspector);
        inspector->selectWindow(0);
        QTest::qWait(1);
    }
    void cleanup()
    {
        delete view;
        QTest::qWait(1);
    }

    void testModelsReparent()
    {
        view->setSource(QUrl(QStringLiteral("qrc:/manual/reparenttest.qml")));
        QTest::qWait(20); // wait at least one frame

        QTest::keyClick(view, Qt::Key_Right);
        QTest::qWait(20);
        QTest::keyClick(view, Qt::Key_Left);
        QTest::qWait(20);
        QTest::keyClick(view, Qt::Key_Right);
        QTest::qWait(20);
    }

    void testModelsCreateDestroy()
    {
        view->setSource(QUrl(QStringLiteral("qrc:/manual/quickitemcreatedestroytest.qml")));
        QTest::qWait(20); // wait at least one frame

        // scroll through the list, to trigger creations/destructions
        for (int i = 0; i < 30; ++i)
            QTest::keyClick(view, Qt::Key_Down);
        QTest::qWait(20);
        for (int i = 0; i < 30; ++i)
            QTest::keyClick(view, Qt::Key_Up);
        QTest::qWait(20);
    }

    void testModelsCreateDestroyProxy()
    {
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
    }

    void testItemPicking()
    {
        auto toolModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ToolModel"));
        QVERIFY(toolModel);

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
    }

    void testFetchingPreview()
    {
        inspector->setSceneViewActive(true);

        QSignalSpy renderSpy(view, SIGNAL(frameSwapped()));
        QVERIFY(renderSpy.isValid());

        QSignalSpy gotFrameSpy(inspector, SIGNAL(sceneRendered(GammaRay::TransferImage,GammaRay::QuickItemGeometry)));
        QVERIFY(gotFrameSpy.isValid());

        view->setSource(QUrl(QStringLiteral("qrc:/manual/reparenttest.qml")));

        QTest::qWait(20); // wait at least one frame
        inspector->renderScene();
        renderSpy.wait(1000);
        gotFrameSpy.wait(1000);

        QVERIFY(renderSpy.size() >= 1);
        QVERIFY(gotFrameSpy.size() >= 1);
        TransferImage tImg = gotFrameSpy.first().first().value<TransferImage>();
        QImage img = tImg.image();

        QVERIFY(!img.isNull());
        QCOMPARE(img.width(), 320);
        QCOMPARE(img.height(), 160);
        QCOMPARE(img.pixel(1,1), QColor(QStringLiteral("lightsteelblue")).rgb());
    }

    void testCustomRenderModes()
    {
        QSignalSpy featureSpy(inspector, SIGNAL(features(GammaRay::QuickInspectorInterface::Features)));
        QVERIFY(featureSpy.isValid());
        inspector->checkFeatures();
        QCOMPARE(featureSpy.size(), 1);
        auto features = featureSpy.at(0).at(0).value<GammaRay::QuickInspectorInterface::Features>();

        QSignalSpy renderSpy(view, SIGNAL(frameSwapped()));
        QVERIFY(renderSpy.isValid());

        view->setSource(QUrl(QStringLiteral("qrc:/manual/reparenttest.qml")));
        renderSpy.wait(1000);

        if (features & QuickInspectorInterface::CustomRenderModeClipping) {
            // We can't do more than making sure, it doesn't crash. Let's wait some frames
            inspector->setCustomRenderMode(QuickInspectorInterface::VisualizeClipping);
            for (int i = 0; i < 3; i++) {
                view->update();
                renderSpy.wait(1000);
            }
        }

        if (features & QuickInspectorInterface::CustomRenderModeOverdraw) {
            inspector->setCustomRenderMode(QuickInspectorInterface::VisualizeOverdraw);
            for (int i = 0; i < 3; i++) {
                renderSpy.wait(1000);
            }
        }

        if (features & QuickInspectorInterface::CustomRenderModeBatches) {
            inspector->setCustomRenderMode(QuickInspectorInterface::VisualizeBatches);
            for (int i = 0; i < 3; i++) {
                view->update();
                renderSpy.wait(1000);
            }
        }

        if (features & QuickInspectorInterface::CustomRenderModeChanges) {
            inspector->setCustomRenderMode(QuickInspectorInterface::VisualizeChanges);
            for (int i = 0; i < 3; i++) {
                view->update();
                renderSpy.wait(1000);
            }
        }

        inspector->setCustomRenderMode(QuickInspectorInterface::NormalRendering);
        for (int i = 0; i < 3; i++) {
            view->update();
          renderSpy.wait(1000);
        }
    }

private:
    QQuickView *view;
    QAbstractItemModel *itemModel;
    QAbstractItemModel *sgModel;
    QuickInspectorInterface *inspector;
};

QTEST_MAIN(QuickInspectorTest)

#include "quickinspectortest.moc"
