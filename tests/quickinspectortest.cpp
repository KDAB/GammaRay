/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <core/toolmanager.h>
#include <common/paths.h>
#include <common/objectbroker.h>
#include <common/remoteviewinterface.h>
#include <common/remoteviewframe.h>

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
        qputenv("GAMMARAY_ProbePath", Paths::probePath(GAMMARAY_PROBE_ABI).toUtf8());
        Hooks::installHooks();
        Probe::startupHookReceived();
        new ProbeCreator(ProbeCreator::Create);
        QTest::qWait(1); // event loop re-entry
    }

    void triggerSceneChange()
    {
        QTest::keyClick(view, Qt::Key_Right);
        QTest::qWait(20);
        QTest::keyClick(view, Qt::Key_Left);
        QTest::qWait(20);
        QTest::keyClick(view, Qt::Key_Right);
    }

    bool waitForSignal(QSignalSpy *spy, bool keepResult = false)
    {
        if (spy->isEmpty())
            spy->wait(1000);
        bool result = !spy->isEmpty();
        if (!keepResult)
            spy->clear();
        return result;
    }

    bool showSource(const QString &sourceFile)
    {
        QSignalSpy renderSpy(view, SIGNAL(frameSwapped()));
        Q_ASSERT(renderSpy.isValid());

        view->setSource(QUrl(sourceFile));
        view->show();
        exposed = QTest::qWaitForWindowExposed(view);
        if (!exposed)
            qWarning()
                <<
            "Unable to expose window, probably running tests on a headless system - ignoring all following render failures.";


        // wait at least two frames so we have the final window size with all render loop/driver combinations...
        QTest::qWait(20);
        waitForSignal(&renderSpy);
        view->update();
        return !exposed || waitForSignal(&renderSpy);
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
        view->setResizeMode(QQuickView::SizeViewToRootObject);
        QTest::qWait(1); // event loop re-entry

        itemModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickItemModel"));
        QVERIFY(itemModel);
        ModelTest itemModelTest(itemModel);

        sgModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickSceneGraphModel"));
        QVERIFY(sgModel);
        ModelTest sgModelTest(sgModel);

        inspector = ObjectBroker::object<QuickInspectorInterface *>();
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
        QVERIFY(showSource(QStringLiteral("qrc:/manual/reparenttest.qml")));

        QTest::keyClick(view, Qt::Key_Right);
        QTest::qWait(20);
        QTest::keyClick(view, Qt::Key_Left);
        QTest::qWait(20);
        QTest::keyClick(view, Qt::Key_Right);
        QTest::qWait(20);
    }

    void testModelsCreateDestroy()
    {
        QVERIFY(showSource(QStringLiteral("qrc:/manual/quickitemcreatedestroytest.qml")));

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
        QVERIFY(showSource(QStringLiteral("qrc:/manual/quickitemcreatedestroytest.qml")));
        QVERIFY(itemModel->rowCount() > 0);
        QVERIFY(sgModel->rowCount() > 0);

        itemModel->setProperty("filterKeyColumn", -1);
        itemModel->setProperty("filterRegExp",
                               QRegExp("Rect", Qt::CaseInsensitive, QRegExp::FixedString));
        sgModel->setProperty("filterKeyColumn", -1);
        sgModel->setProperty("filterRegExp",
                             QRegExp("Transform", Qt::CaseInsensitive, QRegExp::FixedString));
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
        QVERIFY(showSource(QStringLiteral("qrc:/manual/reparenttest.qml")));

        ToolManagerInterface *toolManager = ObjectBroker::object<ToolManagerInterface *>();
        QSignalSpy toolSpy(toolManager, SIGNAL(toolSelected(QString)));
        QVERIFY(toolSpy.isValid());

        auto itemSelectionModel = ObjectBroker::selectionModel(itemModel);
        QVERIFY(itemSelectionModel);
        QSignalSpy itemSpy(itemSelectionModel, SIGNAL(selectionChanged(QItemSelection,
                                                                       QItemSelection)));
        QVERIFY(itemSpy.isValid());

        auto sgSelectionModel = ObjectBroker::selectionModel(sgModel);
        QVERIFY(sgModel);
        QSignalSpy sgSpy(sgSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)));
        QVERIFY(sgSpy.isValid());

        // auto center-click is broken before https://codereview.qt-project.org/141085/
        QTest::mouseClick(view, Qt::LeftButton, Qt::ShiftModifier | Qt::ControlModifier,
                          QPoint(view->width()/2, view->height()/2));
        QTest::qWait(20);

        QCOMPARE(toolSpy.size(), 1);
        QCOMPARE(itemSpy.size(), 1);
        if (!exposed)
            return;
        QCOMPARE(sgSpy.size(), 1);
    }

    void testFetchingPreview()
    {
        auto remoteView
            = ObjectBroker::object<RemoteViewInterface *>(QStringLiteral(
                                                              "com.kdab.GammaRay.QuickRemoteView"));
        QVERIFY(remoteView);
        remoteView->setViewActive(true);

        QSignalSpy renderSpy(view, SIGNAL(frameSwapped()));
        QVERIFY(renderSpy.isValid());

        QSignalSpy gotFrameSpy(remoteView, SIGNAL(frameUpdated(GammaRay::RemoteViewFrame)));
        QVERIFY(gotFrameSpy.isValid());

        QVERIFY(showSource(QStringLiteral("qrc:/manual/reparenttest.qml")));

        remoteView->clientViewUpdated();
        if (!exposed)
            return;
        QVERIFY(waitForSignal(&gotFrameSpy, true));

        QVERIFY(renderSpy.size() >= 1);
        QVERIFY(gotFrameSpy.size() >= 1);
        const auto frame = gotFrameSpy.at(0).at(0).value<RemoteViewFrame>();
        QImage img = frame.image();

        QVERIFY(!img.isNull());
        QCOMPARE(img.width(), 320);
        QCOMPARE(img.height(), 160);
#ifndef Q_OS_WIN // this is too unstable on the CI, rendered results seem to differ in color!?
        QCOMPARE(img.pixel(1, 1), QColor(QStringLiteral("lightsteelblue")).rgb());
#endif

        remoteView->setViewActive(false);
    }

    void testCustomRenderModes()
    {
        QSignalSpy featureSpy(inspector, SIGNAL(features(
                                                    GammaRay::QuickInspectorInterface::Features)));
        QVERIFY(featureSpy.isValid());
        inspector->checkFeatures();
        QCOMPARE(featureSpy.size(), 1);
        auto features = featureSpy.at(0).at(0).value<GammaRay::QuickInspectorInterface::Features>();

        QSignalSpy renderSpy(view, SIGNAL(frameSwapped()));
        QVERIFY(renderSpy.isValid());

        QVERIFY(showSource(QStringLiteral("qrc:/manual/reparenttest.qml")));

        if (features & QuickInspectorInterface::CustomRenderModeClipping) {
            // We can't do more than making sure, it doesn't crash. Let's wait some frames
            inspector->setCustomRenderMode(QuickInspectorInterface::VisualizeClipping);
            for (int i = 0; i < 3; i++)
                triggerSceneChange();
            if (exposed)
                QVERIFY(waitForSignal(&renderSpy));
        }

        if (features & QuickInspectorInterface::CustomRenderModeOverdraw) {
            inspector->setCustomRenderMode(QuickInspectorInterface::VisualizeOverdraw);
            for (int i = 0; i < 3; i++)
                triggerSceneChange();
            if (exposed)
                QVERIFY(waitForSignal(&renderSpy));
        }

        if (features & QuickInspectorInterface::CustomRenderModeBatches) {
            inspector->setCustomRenderMode(QuickInspectorInterface::VisualizeBatches);
            for (int i = 0; i < 3; i++)
                triggerSceneChange();
            if (exposed)
                QVERIFY(waitForSignal(&renderSpy));
        }

        if (features & QuickInspectorInterface::CustomRenderModeChanges) {
            inspector->setCustomRenderMode(QuickInspectorInterface::VisualizeChanges);
            for (int i = 0; i < 3; i++)
                triggerSceneChange();
            if (exposed)
                QVERIFY(waitForSignal(&renderSpy));
        }

        inspector->setCustomRenderMode(QuickInspectorInterface::NormalRendering);
        for (int i = 0; i < 3; i++)
            triggerSceneChange();
        if (exposed)
            QVERIFY(waitForSignal(&renderSpy));
    }

private:
    QQuickView *view;
    QAbstractItemModel *itemModel;
    QAbstractItemModel *sgModel;
    QuickInspectorInterface *inspector;
    bool exposed;
};

QTEST_MAIN(QuickInspectorTest)

#include "quickinspectortest.moc"
