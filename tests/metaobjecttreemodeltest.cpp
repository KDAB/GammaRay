/*
  metaobjecttreemodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
        QTest::qWait(150); // wait for compressed change signals
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
