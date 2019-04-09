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

#include "baseprobetest.h"

#include <common/objectbroker.h>

#include <3rdparty/qt/modeltest.h>

#include <QAbstractItemModel>
#include <QWidget>

using namespace GammaRay;

class WidgetTest : public BaseProbeTest
{
    Q_OBJECT
private:
    int visibleRowCount(QAbstractItemModel *model)
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
};

QTEST_MAIN(WidgetTest)

#include "widgettest.moc"
