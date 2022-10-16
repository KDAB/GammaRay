/*
  objectinstancetest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <core/objectinstance.h>

#include <QDebug>
#include <QTest>

Q_DECLARE_METATYPE(QDateTime *)

struct CustomType
{
};
Q_DECLARE_METATYPE(CustomType)
Q_DECLARE_METATYPE(CustomType *)

using namespace GammaRay;

class ObjectInstanceTest : public QObject
{
    Q_OBJECT
private slots:
    static void testQtObject()
    {
        QObject obj;

        ObjectInstance oi(QVariant::fromValue(&obj));
        QCOMPARE(oi.type(), ObjectInstance::QtObject);
        QCOMPARE(oi.typeName(), QByteArray("QObject"));
        QCOMPARE(oi.object(), &obj);
        QCOMPARE(oi.qtObject(), &obj);
        QCOMPARE(oi.metaObject(), &QObject::staticMetaObject);
        QVERIFY(!oi.isValueType());

        auto oi2 = oi;
        QCOMPARE(oi2.type(), ObjectInstance::QtObject);
        QCOMPARE(oi.object(), oi2.object());
    }

    static void testMetaObjectVariantPointer()
    {
        QDateTime dt;

        ObjectInstance oi(QVariant::fromValue(&dt));
        QCOMPARE(oi.type(), ObjectInstance::Object);
        QCOMPARE(oi.typeName(), QByteArray("QDateTime*"));
        QVERIFY(oi.object());
        QVERIFY(!oi.isValueType());

        auto oi2 = oi;
        QCOMPARE(oi2.type(), ObjectInstance::Object);
        QCOMPARE(oi.object(), oi2.object());

        QDateTime dt2;
        oi2 = ObjectInstance(QVariant::fromValue(&dt2));
        QVERIFY(!(oi == oi2));
    }

    static void testMetaObjectVariantValue()
    {
        QDateTime dt;

        ObjectInstance oi(QVariant::fromValue(dt));
        QCOMPARE(oi.type(), ObjectInstance::Value);
        QCOMPARE(oi.typeName(), QByteArray("QDateTime"));
        QVERIFY(oi.object());
        QVERIFY(oi.isValueType());

        auto oi2 = oi;
        QCOMPARE(oi2.type(), ObjectInstance::Value);
        QVERIFY(oi2.object());

        QDateTime dt2;
        oi2 = ObjectInstance(QVariant::fromValue(dt2));
        QCOMPARE(oi2.type(), ObjectInstance::Value);
        QVERIFY(dt == dt2);
        QVERIFY(oi.variant() == oi2.variant());
        QVERIFY(oi == oi2);
    }

    static void testUnknownVariantValue()
    {
        CustomType t;

        ObjectInstance oi(QVariant::fromValue(t));
        QCOMPARE(oi.type(), ObjectInstance::QtVariant);
        QCOMPARE(oi.typeName(), QByteArray("CustomType"));
    }

    static void testUnknownVariantPointer()
    {
        CustomType t;

        ObjectInstance oi(QVariant::fromValue(&t));
        QCOMPARE(oi.type(), ObjectInstance::QtVariant);
        QCOMPARE(oi.typeName(), QByteArray("CustomType*"));
    }
};

QTEST_MAIN(ObjectInstanceTest)

#include "objectinstancetest.moc"
