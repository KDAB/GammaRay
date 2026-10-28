/*
  metaobjecttest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <core/metaobjectrepository.h>
#include <core/enumrepositoryserver.h>
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
    void initTestCase()
    {
        EnumRepositoryServer::create(this);
    }

    // clearing the repository has to forget everything, it is set up again with new meta objects
    static void testClear()
    {
        QThread thread;
        // the old entries only cause trouble when new meta objects get their addresses
        for (int i = 0; i < 100; ++i) {
            MetaObjectRepository::instance()->clear();
            auto repository = MetaObjectRepository::instance();
            void *obj = static_cast<QObject *>(&thread);
            auto mo = repository->metaObject(repository->metaObject(QStringLiteral("QObject")), obj);
            QVERIFY(mo);
            QCOMPARE(mo, repository->metaObject(QStringLiteral("QThread")));
            QCOMPARE(obj, static_cast<void *>(&thread));
        }
    }

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
        if (!prop)
            return; // to silence clang-tidy

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
        if (!prop)
            return; // to silence clang-tidy
        QCOMPARE(prop->name(), "libraryPaths");
        QCOMPARE(prop->typeName(), "QStringList");
        QCOMPARE(prop->isReadOnly(), true);
        QCOMPARE(prop->value(nullptr).toStringList(), QCoreApplication::libraryPaths());
    }
};

QTEST_MAIN(MetaObjectTest)

#include "metaobjecttest.moc"
