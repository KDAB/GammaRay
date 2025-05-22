/*
  widgettest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "baseprobetest.h"

#include <common/objectbroker.h>
#include <core/paintbuffer.h>

#include <QAbstractItemModelTester>
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
        std::unique_ptr<QWidget> w1(new QWidget);
        QTest::qWait(1); // event loop re-entry

        auto *model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.WidgetTree"));
        QVERIFY(model);
        QAbstractItemModelTester modelTest(model);

        std::unique_ptr<QWidget> w2(new QWidget);
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(visibleRowCount(model), 2);

        w2->setParent(w1.get());
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(visibleRowCount(model), 1);

        w2->setParent(w1.get());
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(visibleRowCount(model), 1);

        std::unique_ptr<QWidget> w3(new QWidget);
        w2->setParent(w3.get()); // reparent without event loop reentry!
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(visibleRowCount(model), 2);

        w2.reset();
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(visibleRowCount(model), 2);

        w1.reset();
        QTest::qWait(1); // event loop re-entry
        QCOMPARE(visibleRowCount(model), 1);

        w3.reset();
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
