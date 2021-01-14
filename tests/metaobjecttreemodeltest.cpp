/*
  metaobjecttreemodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "testhelpers.h"

#include <core/tools/metaobjectbrowser/metaobjecttreemodel.h>
#include <ui/tools/metaobjectbrowser/metaobjecttreeclientproxymodel.h>

#include <common/objectbroker.h>

#include <3rdparty/qt/modeltest.h>

#include <QDebug>

using namespace GammaRay;
using namespace TestHelpers;

class MetaObjectTreeModelTest : public BaseProbeTest
{
    Q_OBJECT
signals:
    void destroyed(); // to trigger the QMO validator

private slots:
    void modelTest()
    {
        createProbe();

        auto srcModel = ObjectBroker::model("com.kdab.GammaRay.MetaObjectBrowserTreeModel");
        QVERIFY(srcModel);
        MetaObjectTreeClientProxyModel model;
        model.setSourceModel(srcModel);

        ModelTest modelTest(&model);
        Probe::instance()->discoverObject(this);
        QTest::qWait(150); // wait for compressed change singals
    }

    void testModelContent()
    {
        createProbe();

        auto srcModel = ObjectBroker::model("com.kdab.GammaRay.MetaObjectBrowserTreeModel");
        QVERIFY(srcModel);
        MetaObjectTreeClientProxyModel model;
        model.setSourceModel(srcModel);
        Probe::instance()->discoverObject(this);

        const auto l = searchFixedIndexes(&model, QLatin1String("MetaObjectTreeModelTest"), Qt::MatchRecursive);
        QCOMPARE(l.size(), 1);
        auto idx = l.at(0);
        QVERIFY(idx.isValid());

        QVERIFY(!idx.data(Qt::DecorationRole).isNull());
        QVERIFY(!idx.data(Qt::ToolTipRole).toString().isEmpty());

        idx = idx.sibling(idx.row(), 1);
        QCOMPARE(idx.data().toInt(), 1);
        QVERIFY(!idx.data(Qt::BackgroundRole).isNull());
        QVERIFY(!idx.data(Qt::ToolTipRole).toString().isEmpty());

        idx = idx.sibling(idx.row(), 2);
        QCOMPARE(idx.data().toInt(), 1);
        QVERIFY(!idx.data(Qt::BackgroundRole).isNull());
        QVERIFY(!idx.data(Qt::ToolTipRole).toString().isEmpty());

        idx = idx.parent(); // BaseProbeTest
        QVERIFY(idx.isValid());
        idx = idx.parent(); // QObject
        QVERIFY(idx.isValid());
        QCOMPARE(idx.data(Qt::DisplayRole).toString(), QLatin1String("QObject"));
        QVERIFY(idx.data(Qt::DecorationRole).isNull());
        QVERIFY(idx.data(Qt::ToolTipRole).toString().isEmpty());

        idx = idx.sibling(idx.row(), 1);
        QVERIFY(idx.data().toInt() >= 0);

        idx = idx.sibling(idx.row(), 2);
        QVERIFY(idx.data().toInt() > 1);
        QVERIFY(idx.data(Qt::BackgroundRole).isNull());
        QVERIFY(idx.data(Qt::ToolTipRole).toString().isEmpty());

        QVERIFY(!idx.parent().isValid());
    }
};

QTEST_MAIN(MetaObjectTreeModelTest)

#include "metaobjecttreemodeltest.moc"
