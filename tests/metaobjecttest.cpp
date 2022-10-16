/*
  metaobjecttest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <core/metaobjectrepository.h>
#include <core/metaobject.h>

#include <QDebug>
#include <QObject>
#include <QThread>
#include <QTest>

Q_DECLARE_METATYPE(QThread::Priority)

using namespace GammaRay;

class MetaObjectTest : public QObject
{
    Q_OBJECT
private slots:
    static void testMetaObject()
    {
        QVERIFY(MetaObjectRepository::instance()->hasMetaObject(QStringLiteral("QThread")));
        auto *mo = MetaObjectRepository::instance()->metaObject(QStringLiteral("QThread"));

        QVERIFY(mo);
        QCOMPARE(mo->className(), QStringLiteral("QThread"));
        QVERIFY(mo->inherits(QStringLiteral("QObject")));

        auto *superMo = mo->superClass(0);
        QVERIFY(superMo);
        QCOMPARE(superMo->className(), QStringLiteral("QObject"));

        QVERIFY(!mo->superClass(1));
        QVERIFY(!superMo->superClass(0));
    }

    static void testMemberProperty()
    {
        auto *mo = MetaObjectRepository::instance()->metaObject(QStringLiteral("QThread"));
        QVERIFY(mo->propertyCount() >= 7); // depends on Qt version

        MetaProperty *prop = nullptr;
        for (int i = 0; i < mo->propertyCount(); ++i) {
            prop = mo->propertyAt(i);
            QVERIFY(prop);
            if (strcmp(prop->name(), "priority") == 0)
                break;
        }

        QVERIFY(prop);
        QCOMPARE(prop->name(), "priority");
        QCOMPARE(prop->typeName(), "QThread::Priority");

        QThread t;
        QCOMPARE(prop->value(&t).value<QThread::Priority>(), t.priority());
        QCOMPARE(prop->isReadOnly(), false);
    }

    static void testStaticProperty()
    {
        auto *mo = MetaObjectRepository::instance()->metaObject(QStringLiteral("QCoreApplication"));
        QVERIFY(mo);
        QVERIFY(mo->propertyCount() >= 8); // depends on Qt version

        MetaProperty *prop = nullptr;
        for (int i = 0; i < mo->propertyCount(); ++i) {
            prop = mo->propertyAt(i);
            QVERIFY(prop);
            if (strcmp(prop->name(), "libraryPaths") == 0)
                break;
        }

        QVERIFY(prop);
        QCOMPARE(prop->name(), "libraryPaths");
        QCOMPARE(prop->typeName(), "QStringList");
        QCOMPARE(prop->isReadOnly(), true);
        QCOMPARE(prop->value(nullptr).toStringList(), QCoreApplication::libraryPaths());
    }
};

QTEST_MAIN(MetaObjectTest)

#include "metaobjecttest.moc"
