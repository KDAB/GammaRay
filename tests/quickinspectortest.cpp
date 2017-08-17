/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "basequicktest.h"
#include "testhelpers.h"

#include <plugins/quickinspector/quickinspectorinterface.h>
#include <core/toolmanager.h>
#include <common/objectbroker.h>
#include <common/remoteviewinterface.h>
#include <common/remoteviewframe.h>

#include <3rdparty/qt/modeltest.h>

#include <QItemSelectionModel>
#include <QRegExp>

#if QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
Q_DECLARE_METATYPE(QItemSelection)
#endif

using namespace GammaRay;
using namespace TestHelpers;

class QuickInspectorTest : public BaseQuickTest
{
    Q_OBJECT
protected:
    bool ignoreNonExposedView() const override
    {
        return true;
    }

private:
    void triggerSceneChange()
    {
        QTest::keyClick(view(), Qt::Key_Right);
        QTest::qWait(20);
        QTest::keyClick(view(), Qt::Key_Left);
        QTest::qWait(20);
        QTest::keyClick(view(), Qt::Key_Right);
    }

private slots:
    void initTestCase()
    {
        qRegisterMetaType<QItemSelection>();
    }

    void init() override
    {
        BaseQuickTest::init();

        itemModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickItemModel"));
        QVERIFY(itemModel);
        new ModelTest(itemModel, view());

        sgModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.QuickSceneGraphModel"));
        QVERIFY(sgModel);
        new ModelTest(sgModel, view());

        inspector = ObjectBroker::object<QuickInspectorInterface *>();
        QVERIFY(inspector);
        inspector->selectWindow(0);
        QTest::qWait(1);
    }

    void testModelsReparent()
    {
        QVERIFY(showSource(QStringLiteral("qrc:/manual/reparenttest.qml")));

        QTest::keyClick(view(), Qt::Key_Right);
        QTest::qWait(20);
        QTest::keyClick(view(), Qt::Key_Left);
        QTest::qWait(20);
        QTest::keyClick(view(), Qt::Key_Right);
        QTest::qWait(20);
    }

    void testModelsCreateDestroy()
    {
        QVERIFY(showSource(QStringLiteral("qrc:/manual/quickitemcreatedestroytest.qml")));

        // scroll through the list, to trigger creations/destructions
        for (int i = 0; i < 30; ++i)
            QTest::keyClick(view(), Qt::Key_Down);
        QTest::qWait(20);
        for (int i = 0; i < 30; ++i)
            QTest::keyClick(view(), Qt::Key_Up);
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
            QTest::keyClick(view(), Qt::Key_Down);
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
        QSignalSpy itemSpy(itemSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)));
        QVERIFY(itemSpy.isValid());

        auto sgSelectionModel = ObjectBroker::selectionModel(sgModel);
        QVERIFY(sgModel);
        QSignalSpy sgSpy(sgSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)));
        QVERIFY(sgSpy.isValid());

        // auto center-click is broken before https://codereview.qt-project.org/141085/
        QTest::mouseClick(view(),
                          Qt::LeftButton,
                          Qt::ShiftModifier | Qt::ControlModifier,
                          QPoint(view()->width() / 2, view()->height() / 2));
        QTest::qWait(20);

        QCOMPARE(toolSpy.size(), 1);
        QCOMPARE(itemSpy.size(), 1);
        if (!isViewExposed())
            return;

        QCOMPARE(sgSpy.size(), 1);
    }

    void testFetchingPreview()
    {
        auto remoteView =
            ObjectBroker::object<RemoteViewInterface *>(
                QStringLiteral("com.kdab.GammaRay.QuickRemoteView"));

        QVERIFY(remoteView);
        remoteView->setViewActive(true);

        QSignalSpy renderSpy(view(), SIGNAL(frameSwapped()));
        QVERIFY(renderSpy.isValid());

        QSignalSpy gotFrameSpy(remoteView, SIGNAL(frameUpdated(GammaRay::RemoteViewFrame)));
        QVERIFY(gotFrameSpy.isValid());

        QVERIFY(showSource(QStringLiteral("qrc:/manual/rotationinvariant.qml")));

        remoteView->clientViewUpdated();
        if (!isViewExposed())
            return;

        QVERIFY(waitForSignal(&gotFrameSpy, true));

        QVERIFY(renderSpy.size() >= 1);
        QVERIFY(gotFrameSpy.size() >= 1);
        const auto frame = gotFrameSpy.at(0).at(0).value<RemoteViewFrame>();
        QImage img = frame.image();
        QTransform transform = frame.transform();

        img = img.transformed(transform);

        QVERIFY(!img.isNull());
        QCOMPARE(img.width(), static_cast<int>(view()->width() *view()->devicePixelRatio()));
        QCOMPARE(img.height(), static_cast<int>(view()->height() *view()->devicePixelRatio()));
#ifndef Q_OS_WIN // this is too unstable on the CI, rendered results seem to differ in color!?
        QCOMPARE(img.pixel(1, 1), QColor(255, 0, 0).rgb());
        QCOMPARE(img.pixel(99, 1), QColor(0, 255, 0).rgb());
        QCOMPARE(img.pixel(1, 99), QColor(0, 0, 255).rgb());
        QCOMPARE(img.pixel(99, 99), QColor(255, 255, 0).rgb());
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

        QSignalSpy renderSpy(view(), SIGNAL(frameSwapped()));
        QVERIFY(renderSpy.isValid());

        QVERIFY(showSource(QStringLiteral("qrc:/manual/reparenttest.qml")));

        if (features & QuickInspectorInterface::CustomRenderModeClipping) {
            // We can't do more than making sure, it doesn't crash. Let's wait some frames
            inspector->setCustomRenderMode(QuickInspectorInterface::VisualizeClipping);
            for (int i = 0; i < 3; i++)
                triggerSceneChange();
            if (isViewExposed())
                QVERIFY(waitForSignal(&renderSpy));
        }

        if (features & QuickInspectorInterface::CustomRenderModeOverdraw) {
            inspector->setCustomRenderMode(QuickInspectorInterface::VisualizeOverdraw);
            for (int i = 0; i < 3; i++)
                triggerSceneChange();
            if (isViewExposed())
                QVERIFY(waitForSignal(&renderSpy));
        }

        if (features & QuickInspectorInterface::CustomRenderModeBatches) {
            inspector->setCustomRenderMode(QuickInspectorInterface::VisualizeBatches);
            for (int i = 0; i < 3; i++)
                triggerSceneChange();
            if (isViewExposed())
                QVERIFY(waitForSignal(&renderSpy));
        }

        if (features & QuickInspectorInterface::CustomRenderModeChanges) {
            inspector->setCustomRenderMode(QuickInspectorInterface::VisualizeChanges);
            for (int i = 0; i < 3; i++)
                triggerSceneChange();
            if (isViewExposed())
                QVERIFY(waitForSignal(&renderSpy));
        }

        inspector->setCustomRenderMode(QuickInspectorInterface::NormalRendering);
        for (int i = 0; i < 3; i++)
            triggerSceneChange();
        if (isViewExposed())
            QVERIFY(waitForSignal(&renderSpy));
    }

private:
    QAbstractItemModel *itemModel;
    QAbstractItemModel *sgModel;
    QuickInspectorInterface *inspector;
};

QTEST_MAIN(QuickInspectorTest)

#include "quickinspectortest.moc"
