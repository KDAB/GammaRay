/*
  objectinstancetest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <core/objectinstance.h>

#include <QDebug>
#include <QtTest/qtest.h>

Q_DECLARE_METATYPE(QDateTime *)

struct CustomType {};
Q_DECLARE_METATYPE(CustomType)
Q_DECLARE_METATYPE(CustomType *)

using namespace GammaRay;

class ObjectInstanceTest : public QObject
{
    Q_OBJECT
private slots:
    void testQtObject()
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

    void testMetaObjectVariantPointer()
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

    void testMetaObjectVariantValue()
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

    void testUnknownVariantValue()
    {
        CustomType t;

        ObjectInstance oi(QVariant::fromValue(t));
        QCOMPARE(oi.type(), ObjectInstance::QtVariant);
        QCOMPARE(oi.typeName(), QByteArray("CustomType"));
    }

    void testUnknownVariantPointer()
    {
        CustomType t;

        ObjectInstance oi(QVariant::fromValue(&t));
        QCOMPARE(oi.type(), ObjectInstance::QtVariant);
        QCOMPARE(oi.typeName(), QByteArray("CustomType*"));
    }
};

QTEST_MAIN(ObjectInstanceTest)

#include "objectinstancetest.moc"
