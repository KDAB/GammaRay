/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Christoph Sterz <christoph.sterz@kdab.com>

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

using namespace GammaRay;

class QuickInspectorPickingTest : public QObject
{
    Q_OBJECT
private:
    void createProbe()
    {
        Paths::setRelativeRootPath(GAMMARAY_INVERSE_BIN_DIR);
        qputenv("GAMMARAY_ProbePath", Paths::probePath(GAMMARAY_PROBE_ABI).toUtf8());
        qputenv("GAMMARAY_ServerAddress", GAMMARAY_DEFAULT_LOCAL_TCP_URL);
        Hooks::installHooks();
        Probe::startupHookReceived();
        new ProbeCreator(ProbeCreator::Create);
        QTest::qWait(1); // event loop re-entry
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

    void testItemPicking_data()
    {
        QTest::addColumn<QString>("qmlFile", nullptr);
        QTest::addColumn<QString>("pickedObjectId", nullptr);

        QTest::newRow("Vanilla Rect-Clicking") << "qrc:/manual/picking/stackedrects.qml" << "bluerect";
        QTest::newRow("Negative z-order") << "qrc:/manual/picking/negativezordering.qml" << "greenrect";
        QTest::newRow("Invisible overlay") << "qrc:/manual/picking/invisibleoverlay.qml" << "redrect";
        QTest::newRow("Opacity:0 overlay") << "qrc:/manual/picking/opacityzerooverlay.qml" << "yellowrect";
        QTest::newRow("Loader") << "qrc:/manual/picking/loader.qml" << "bluerect";
        QTest::newRow("Outside of parent") << "qrc:/manual/picking/outsideofparent.qml" << "redrectchild";
    }

    // Info: Clickposition is always in Center of View
    void testItemPicking()
    {
        QFETCH(QString, qmlFile);
        QFETCH(QString, pickedObjectId);

        QVERIFY(showSource(qmlFile));

        auto itemSelectionModel = ObjectBroker::selectionModel(itemModel);
        QVERIFY(itemSelectionModel);
        QSignalSpy itemSpy(itemSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)));
        QVERIFY(itemSpy.isValid());

        // auto center-click is broken before https://codereview.qt-project.org/141085/
        QTest::mouseClick(view, Qt::LeftButton, Qt::ShiftModifier | Qt::ControlModifier,
                          QPoint(view->width()/2, view->height()/2));

        if (itemSpy.isEmpty())
            QVERIFY(itemSpy.wait());
        QCOMPARE(itemSpy.size(), 1);

        QItemSelection selectedItem = qvariant_cast<QItemSelection>(itemSpy.at(0).at(0));
        QVariant id = itemModel->data(selectedItem.indexes().first());

        QCOMPARE(id.toString(), pickedObjectId);
    }

private:
    QQuickView *view;
    QAbstractItemModel *itemModel;
    QuickInspectorInterface *inspector;
    bool exposed;
};

QTEST_MAIN(QuickInspectorPickingTest)

#include "quickinspectorpickingtest.moc"
