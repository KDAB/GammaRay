/*
  quicktexturetest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <common/objectbroker.h>
#include <common/remoteviewinterface.h>
#include <common/remoteviewframe.h>

#include <QQuickItem>

using namespace GammaRay;

class QuickTextureTest : public BaseQuickTest
{
    Q_OBJECT
private slots:
    void testTextureGrab()
    {
        if (!showSource("qrc:/manual/textures.qml"))
            return;

        auto remoteView = ObjectBroker::object<RemoteViewInterface*>("com.kdab.GammaRay.ObjectInspector.texture.remoteView");
        QVERIFY(remoteView);
        QSignalSpy frameSpy(remoteView, SIGNAL(frameUpdated(GammaRay::RemoteViewFrame)));
        QVERIFY(frameSpy.isValid());
        remoteView->setViewActive(true);

        auto imageItem = view()->rootObject();
        QVERIFY(imageItem);
        QCOMPARE(imageItem->metaObject()->className(), "QQuickImage");
        Probe::instance()->selectObject(imageItem, QPoint());

        QVERIFY(frameSpy.wait());
        const auto frame = frameSpy.at(0).at(0).value<RemoteViewFrame>();
        QCOMPARE(frame.viewRect(), QRectF(0, 0, 360, 360));
        QCOMPARE(frame.image().pixel(358, 0), QColor(Qt::red).rgb());
        QCOMPARE(frame.image().pixel(119, 0), QColor(Qt::green).rgb());
        QCOMPARE(frame.image().pixel(239, 0), QColor(Qt::blue).rgb());
    }

    void testDFGrab()
    {
        if (!showSource("qrc:/manual/textures.qml"))
            return;

        auto remoteView = ObjectBroker::object<RemoteViewInterface*>("com.kdab.GammaRay.ObjectInspector.texture.remoteView");
        QVERIFY(remoteView);
        QSignalSpy frameSpy(remoteView, SIGNAL(frameUpdated(GammaRay::RemoteViewFrame)));
        QVERIFY(frameSpy.isValid());
        remoteView->setViewActive(true);

        QQuickItem *textItem = nullptr;
        foreach (auto item, view()->rootObject()->findChildren<QQuickItem*>()) {
            if (strcmp(item->metaObject()->className(), "QQuickText") == 0)
                textItem = item;
        }
        QVERIFY(textItem);
        Probe::instance()->selectObject(textItem, QPoint());

        QVERIFY(frameSpy.wait());
        const auto frame = frameSpy.at(0).at(0).value<RemoteViewFrame>();
        QVERIFY(frame.viewRect().height() > 0);
        QVERIFY(frame.viewRect().width() > 0);
    }

    void testShaderEffectSourceGrab()
    {
        if (!showSource("qrc:/manual/shadereffect.qml")) {
            qWarning() << "Skipping test due to unavailable QtQuick display.";
            return;
        }

        auto remoteView = ObjectBroker::object<RemoteViewInterface*>("com.kdab.GammaRay.ObjectInspector.texture.remoteView");
        QVERIFY(remoteView);
        QSignalSpy frameSpy(remoteView, SIGNAL(frameUpdated(GammaRay::RemoteViewFrame)));
        QVERIFY(frameSpy.isValid());
        remoteView->setViewActive(true);

        QQuickItem *source = nullptr;
        foreach (auto item, view()->rootObject()->findChildren<QQuickItem*>()) {
            if (item->inherits("QQuickShaderEffectSource"))
                source = item;
        }
        QVERIFY(source);
        Probe::instance()->selectObject(source, QPoint());

        QVERIFY(frameSpy.wait());
        const auto frame = frameSpy.at(0).at(0).value<RemoteViewFrame>();
        QVERIFY(frame.viewRect().height() > 0);
        QVERIFY(frame.viewRect().width() > 0);
    }
};

QTEST_MAIN(QuickTextureTest)

#include "quicktexturetest.moc"
