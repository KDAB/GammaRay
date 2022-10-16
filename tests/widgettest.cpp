/*
  widgettest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "baseprobetest.h"

#include <common/objectbroker.h>
#include <core/paintbuffer.h>

#include <3rdparty/qt/modeltest.h>

#include <QAbstractItemModel>
#include <QWidget>

using namespace GammaRay;

class WidgetTest : public BaseProbeTest
{
    Q_OBJECT
private:
    static int visibleRowCount(QAbstractItemModel *model)
    {
        int count = 0;
        for (int i = 0; i < model->rowCount(); ++i) {
            auto idx = model->index(i, 1);
            if (!idx.data(Qt::DisplayRole).toString().startsWith(QLatin1String("QDesktop")))
                ++count;
        }
        return count;
    }

private slots:
    void testWidgetReparent()
    {
        createProbe();

        // we need one widget for the plugin to activate, otherwise the model will not be available
        auto w1 = new QWidget;
        QTest::qWait(1); // event loop re-entry

        auto *model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.WidgetTree"));
        QVERIFY(model);
        ModelTest modelTest(model);

        auto w2 = new QWidget;
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(visibleRowCount(model), 2);

        w2->setParent(w1);
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(visibleRowCount(model), 1);

        w2->setParent(w1);
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(visibleRowCount(model), 1);

        auto w3 = new QWidget;
        w2->setParent(w3); // reparent without event loop reentry!
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(visibleRowCount(model), 2);

        delete w2;
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(visibleRowCount(model), 2);

        delete w1;
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(visibleRowCount(model), 1);

        delete w3;
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(visibleRowCount(model), 0);
    }

    void testPaintBuffer()
    {
        PaintBuffer buffer;
        auto buffer2 = buffer;
        buffer = PaintBuffer();
    }
};

QTEST_MAIN(WidgetTest)

#include "widgettest.moc"
