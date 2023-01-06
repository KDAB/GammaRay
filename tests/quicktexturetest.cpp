/*
  quicktexturetest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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

        auto remoteView = ObjectBroker::object<RemoteViewInterface *>("com.kdab.GammaRay.ObjectInspector.texture.remoteView");
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

        auto remoteView = ObjectBroker::object<RemoteViewInterface *>("com.kdab.GammaRay.ObjectInspector.texture.remoteView");
        QVERIFY(remoteView);
        QSignalSpy frameSpy(remoteView, SIGNAL(frameUpdated(GammaRay::RemoteViewFrame)));
        QVERIFY(frameSpy.isValid());
        remoteView->setViewActive(true);

        QQuickItem *textItem = nullptr;
        foreach (auto item, view()->rootObject()->findChildren<QQuickItem *>()) {
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

        auto remoteView = ObjectBroker::object<RemoteViewInterface *>("com.kdab.GammaRay.ObjectInspector.texture.remoteView");
        QVERIFY(remoteView);
        QSignalSpy frameSpy(remoteView, SIGNAL(frameUpdated(GammaRay::RemoteViewFrame)));
        QVERIFY(frameSpy.isValid());
        remoteView->setViewActive(true);

        QQuickItem *source = nullptr;
        foreach (auto item, view()->rootObject()->findChildren<QQuickItem *>()) {
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
