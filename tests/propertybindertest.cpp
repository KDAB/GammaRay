/*
  propertybindertest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <ui/propertybinder.h>

#include <QObject>
#include <QTest>

class MyObject : public QObject
{
    Q_PROPERTY(int intProp READ intProp WRITE setIntProp NOTIFY intPropChanged)
    Q_PROPERTY(int intProp2 READ intProp2 WRITE setIntProp2 NOTIFY intProp2Changed)
    Q_OBJECT
public:
    explicit MyObject(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
    int intProp()
    {
        return p1;
    }
    int intProp2()
    {
        return p2;
    }
    void setIntProp(int i)
    {
        if (p1 == i)
            return;
        p1 = i;
        emit intPropChanged();
    }

    void setIntProp2(int i)
    {
        if (p2 == i)
            return;
        p2 = i;
        emit intProp2Changed();
    }

signals:
    void intPropChanged();
    void intProp2Changed();

private:
    int p1 = 0, p2 = 23;
};

using namespace GammaRay;

class PropertyBinderTest : public QObject
{
    Q_OBJECT
private slots:
    void testBinding()
    {
        auto *obj1 = new MyObject(this);
        auto *obj2 = new MyObject(this);
        new PropertyBinder(obj1, "intProp", obj2, "intProp");

        obj1->setIntProp(5);
        QCOMPARE(obj2->intProp(), 5);

        obj2->setIntProp(9);
        QCOMPARE(obj1->intProp(), 9);

        delete obj2;
        obj1->setIntProp(42); // don't crash
    }

    void testInitialBinding()
    {
        auto *obj1 = new MyObject(this);
        obj1->setIntProp(18);
        auto *obj2 = new MyObject(this);
        QVERIFY(obj1->intProp() != obj2->intProp());
        new PropertyBinder(obj1, "intProp", obj2, "intProp");
        QCOMPARE(obj2->intProp(), 18);
    }

    void testMultiBinding()
    {
        auto *obj1 = new MyObject(this);
        obj1->setIntProp(18);
        obj1->setIntProp2(133);
        auto *obj2 = new MyObject(this);

        auto binder = new PropertyBinder(obj1, obj2);
        binder->add("intProp", "intProp");
        binder->add("intProp2", "intProp2");

        QVERIFY(obj1->intProp() != obj2->intProp());
        QVERIFY(obj1->intProp2() != obj2->intProp2());

        binder->syncSourceToDestination();

        QCOMPARE(obj2->intProp(), 18);
        QCOMPARE(obj2->intProp2(), 133);

        obj2->setIntProp(23);
        QCOMPARE(obj1->intProp(), 23);
    }
};

QTEST_MAIN(PropertyBinderTest)

#include "propertybindertest.moc"
