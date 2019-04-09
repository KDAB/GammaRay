/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <common/objectbroker.h>
#include <common/remoteviewinterface.h>
#include <common/remoteviewframe.h>

#include <3rdparty/qt/modeltest.h>

#include <QItemSelectionModel>
#include <QQuickItem>

using namespace GammaRay;
using namespace TestHelpers;

class QuickInspectorTest2 : public BaseQuickTest
{
    Q_OBJECT
protected:
    bool ignoreNonExposedView() const override
    {
        return true;
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
        inspector->setServerSideDecorationsEnabled(false);
        inspector->selectWindow(0);
        QTest::qWait(1);
    }

    void testPreviewFetchingThrottler_data()
    {
        QTest::addColumn<bool>("clientIsReplying", nullptr);
        QTest::newRow("no-reply") << false;
        QTest::newRow("reply") << true;
    }

    void testPreviewFetchingThrottler()
    {
        QFETCH(bool, clientIsReplying);

        auto remoteView
            = ObjectBroker::object<RemoteViewInterface *>(QStringLiteral(
                                                              "com.kdab.GammaRay.QuickRemoteView"));
        QVERIFY(remoteView);

        QVERIFY(showSource(QStringLiteral("qrc:/manual/rotationinvariant.qml")));

        if (!isViewExposed())
            return;

        auto rootItem = view()->rootObject();
        QVERIFY(rootItem);
        Probe::instance()->selectObject(rootItem, QPoint());

        // RemoteViewServer throttle the requests with an interval of qRound(1000.0 / 60.0)
        const qreal throttlerInterval = qRound(1000.0 / 60.0);

        if (clientIsReplying) {
            connect(remoteView, &RemoteViewInterface::frameUpdated,
                    remoteView, &RemoteViewInterface::clientViewUpdated, Qt::UniqueConnection);
        } else {
            disconnect(remoteView, &RemoteViewInterface::frameUpdated,
                       remoteView, &RemoteViewInterface::clientViewUpdated);
        }

        QSignalSpy requestedSpy(remoteView, SIGNAL(requestUpdate()));
        QVERIFY(requestedSpy.isValid());

        QSignalSpy updatedSpy(remoteView, &RemoteViewInterface::frameUpdated);
        QVERIFY(updatedSpy.isValid());

        // Testing static scene only send 1 frame
        for (int i = 0; i < 3; ++i) {
            remoteView->setViewActive(true);
            // Activating the view trigger an update request
            QVERIFY(waitForSignal(&updatedSpy, true));
            QVERIFY(requestedSpy.count() == 1 || requestedSpy.count() == 2); // should be 1, but we might see spurious repaints on windows
            QVERIFY(updatedSpy.count() == 1 || updatedSpy.count() == 2);
            if (!clientIsReplying)
                remoteView->clientViewUpdated();

            requestedSpy.clear();
            updatedSpy.clear();

            if (clientIsReplying) {
                // The client is answering clientViewUpdated automatically.
                // For 1 request and multiple client answers the server should only send 1 frame.

                view()->update();
                QVERIFY(waitForSignal(&updatedSpy, true));

                for (int j = 0; j < 3; ++j) {
                    // The automatical clientViewUpdated answer is only done for frame sent.
                    // Let manually trigger answers.
                    QTest::qWait(throttlerInterval);
                    remoteView->clientViewUpdated();
                }

                QVERIFY(waitForSignal(&requestedSpy, true));
                QVERIFY(requestedSpy.count() == 1 || requestedSpy.count() == 2);
                QVERIFY(updatedSpy.count() == 1 || updatedSpy.count() == 2);
            } else {
                // The client is not answering with clientViewUpdated automatically.
                // Only 1 request and 1 frame sent should trigger.

                for (int i = 0; i < 3; ++i) {
                    view()->update();
                    QTest::qWait(throttlerInterval);
                    QVERIFY(waitForSignal(&updatedSpy, true));
                }

                QVERIFY(requestedSpy.count() == 1 || requestedSpy.count() == 2);
                QVERIFY(updatedSpy.count() == 1 || updatedSpy.count() == 2);
            }

            requestedSpy.clear();
            updatedSpy.clear();

            remoteView->setViewActive(false);
        }

        // Our animation properties
        const qreal animationInterval = throttlerInterval;
        const qreal animationDuration = 1400.0;
        // Qml try to render @ ~60fps
        const qreal maxPossibleQmlRequests =
            clientIsReplying ? animationDuration / 1000.0 * 60.0 : 1.0;
        const qreal maxPossibleThrottledRequests =
            clientIsReplying ?
                qMin(maxPossibleQmlRequests, animationDuration / throttlerInterval) : 1.0;

        // Testing dynamic scene
        for (int i = 0; i < 3; i++) {
            remoteView->setViewActive(true);
            // Activating the view trigger an update request
            QVERIFY(waitForSignal(&updatedSpy, true));
            QCOMPARE(requestedSpy.count(), 1);
            QCOMPARE(updatedSpy.count(), 1);
            if (!clientIsReplying)
                remoteView->clientViewUpdated();

            requestedSpy.clear();
            updatedSpy.clear();

            // We rotate our rendering every animationInterval-ms for animationDuration-ms
            rootItem->setProperty("interval", qRound(animationInterval));
            rootItem->setProperty("animated", true);
            QTest::qWait(qRound(animationDuration));

            // Wait to process pending requests...
            rootItem->setProperty("animated", false);
            QTest::qWait(qRound(animationDuration));

            QVERIFY(requestedSpy.count() <= qRound(maxPossibleThrottledRequests * 1.05) + 1);
            QVERIFY(updatedSpy.count() <= qRound(maxPossibleThrottledRequests * 1.05) + 1);
            QCOMPARE(requestedSpy.count(), updatedSpy.count());

            requestedSpy.clear();
            updatedSpy.clear();

            remoteView->setViewActive(false);
        }
    }

private:
    QAbstractItemModel *itemModel;
    QAbstractItemModel *sgModel;
    QuickInspectorInterface *inspector;
};

QTEST_MAIN(QuickInspectorTest2)

#include "quickinspectortest2.moc"
