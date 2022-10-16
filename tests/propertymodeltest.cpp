/*
  propertymodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "testhelpers.h"
#include "baseprobetest.h"

#include <core/objectinstance.h>
#include <core/aggregatedpropertymodel.h>

#include "shared/propertytestobject.h"

#include <3rdparty/qt/modeltest.h>

#include <QDebug>
#include <QObject>
#include <QSignalSpy>
#include <QTest>

using namespace GammaRay;
using namespace TestHelpers;

class PropertyModelTest : public BaseProbeTest
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        createProbe();
    }

    static void testPropertyModel()
    {
        PropertyTestObject obj;
        obj.setProperty("dynamicProperty", 5);

        AggregatedPropertyModel model;
        ModelTest modelTest(&model);
        model.setObject(&obj);

        QVERIFY(model.rowCount() > 9);
        auto dynRow = searchFixedIndex(&model, "dynamicProperty");
        QVERIFY(dynRow.isValid());
        QCOMPARE(dynRow.data(Qt::DisplayRole).toString(), QStringLiteral("dynamicProperty"));
        QVERIFY(dynRow.sibling(dynRow.row(), 1).flags() & Qt::ItemIsEditable);
        QCOMPARE(dynRow.sibling(dynRow.row(), 1).data(Qt::DisplayRole).toString(),
                 QStringLiteral("5"));
        QCOMPARE(dynRow.sibling(dynRow.row(), 1).data(Qt::EditRole), QVariant(5));

        auto qmoRow = searchFixedIndex(&model, "intProp");
        QVERIFY(qmoRow.isValid());
        QCOMPARE(qmoRow.data(Qt::DisplayRole).toString(), QStringLiteral("intProp"));
        auto qmoRow2 = qmoRow.sibling(qmoRow.row(), 1);
        QVERIFY(qmoRow2.flags() & Qt::ItemIsEditable);
        QCOMPARE(qmoRow2.data(Qt::DisplayRole).toString(), QStringLiteral("0"));
        QCOMPARE(qmoRow2.data(Qt::EditRole), QVariant(0));
        model.setData(qmoRow2, 12);
        QCOMPARE(obj.intProp(), 12);

        auto moRow = searchFixedIndex(&model, "thread");
        QVERIFY(moRow.isValid());
        QCOMPARE(moRow.data(Qt::DisplayRole).toString(), QStringLiteral("thread"));
        QVERIFY((moRow.sibling(moRow.row(), 1).flags() & Qt::ItemIsEditable) == 0);
        QVERIFY(!moRow.sibling(moRow.row(), 1).data(Qt::DisplayRole).toString().isEmpty());
    }

    static void testMetaObject()
    {
        AggregatedPropertyModel model;
        model.setObject(ObjectInstance(nullptr, &Gadget::staticMetaObject));
        ModelTest modelTest(&model);

        QCOMPARE(model.rowCount(), 1);
        auto qmoRow = searchFixedIndex(&model, "prop1");
        QVERIFY(qmoRow.isValid());
    }

    static void testChangeNotification()
    {
        ChangingPropertyObject obj;
        AggregatedPropertyModel model;
        // ModelTest modelTest(&model);
        model.setObject(&obj);
        QVERIFY(model.rowCount() >= 4);

        QSignalSpy changeSpy(&model, SIGNAL(dataChanged(QModelIndex, QModelIndex)));
        QVERIFY(changeSpy.isValid());
        QSignalSpy addSpy(&model, SIGNAL(rowsInserted(QModelIndex, int, int)));
        QVERIFY(addSpy.isValid());
        QSignalSpy removeSpy(&model, SIGNAL(rowsRemoved(QModelIndex, int, int)));
        QVERIFY(removeSpy.isValid());

        obj.changeProperties();

        QCOMPARE(changeSpy.size(), 1);
        QCOMPARE(addSpy.size(), 1);

        obj.changeProperties();
        QCOMPARE(changeSpy.size(), 3);

        obj.setProperty("dynamicChangingProperty", QVariant());
        QCOMPARE(changeSpy.size(), 3);
        QCOMPARE(addSpy.size(), 1);
        QCOMPARE(removeSpy.size(), 1);
    }

    static void testGadgetRO()
    {
        PropertyTestObject obj;
        AggregatedPropertyModel model;
        model.setObject(ObjectInstance(&obj));

        auto idx = searchFixedIndex(&model, "gadgetReadOnly");
        QVERIFY(idx.isValid());
        QCOMPARE(model.rowCount(idx), 1);
        idx = model.index(0, 1, idx);
        QVERIFY((idx.flags() & Qt::ItemIsEditable) == 0);
    }

    static void testGadgetRW()
    {
        PropertyTestObject obj;
        AggregatedPropertyModel model;
        model.setObject(ObjectInstance(&obj));
        ModelTest modelTest(&model);

        auto idx = searchFixedIndex(&model, "gadget");
        QVERIFY(idx.isValid());
        QCOMPARE(model.rowCount(idx), 1);
        idx = model.index(0, 1, idx);
        QVERIFY(idx.flags() & Qt::ItemIsEditable);
        QVERIFY(model.setData(idx, 1554));
        QCOMPARE(obj.gadgetPointer()->prop1(), 1554);

        idx = searchFixedIndex(&model, "gadgetPointer");
        QVERIFY(idx.isValid());
        QCOMPARE(model.rowCount(idx), 1);
        idx = model.index(0, 1, idx);
        QVERIFY(idx.flags() & Qt::ItemIsEditable);
        QVERIFY(model.setData(idx, 1559));
        QCOMPARE(obj.gadgetPointer()->prop1(), 1559);
    }
};

QTEST_MAIN(PropertyModelTest)

#include "propertymodeltest.moc"
