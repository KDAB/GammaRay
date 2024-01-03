/*
  propertybindertest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <ui/propertybinder.h>

#include <QObject>
#include <QTest>

#include <memory>

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
    int intProp() const
    {
        return p1;
    }
    int intProp2() const
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
        std::unique_ptr<MyObject> obj1(new MyObject(this));
        obj1->setIntProp(18);
        std::unique_ptr<MyObject> obj2(new MyObject(this));
        QVERIFY(obj1->intProp() != obj2->intProp());
        new PropertyBinder(obj1.get(), "intProp", obj2.get(), "intProp");
        QCOMPARE(obj2->intProp(), 18);
    }

    void testMultiBinding()
    {
        std::unique_ptr<MyObject> obj1(new MyObject(this));
        obj1->setIntProp(18);
        obj1->setIntProp2(133);
        std::unique_ptr<MyObject> obj2(new MyObject(this));

        auto binder = new PropertyBinder(obj1.get(), obj2.get());
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
