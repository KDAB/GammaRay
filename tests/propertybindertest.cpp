/*
  propertybindertest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <ui/propertybinder.h>

#include <QtTest/qtest.h>
#include <QObject>

class MyObject : public QObject
{
    Q_PROPERTY(int intProp READ intProp WRITE setIntProp NOTIFY intPropChanged)
    Q_OBJECT
public:
    explicit MyObject(QObject *parent = 0) : QObject(parent), p1(0) {}
    int intProp() { return p1; }
    void setIntProp(int i)
    {
        if (p1 == i)
            return;
        p1 = i;
        emit intPropChanged();
    }

signals:
    void intPropChanged();

private:
    int p1;
};

using namespace GammaRay;

class PropertyBinderTest : public QObject
{
    Q_OBJECT
private slots:
    void testBinding()
    {
        MyObject *obj1 = new MyObject(this);
        MyObject *obj2 = new MyObject(this);
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
        MyObject *obj1 = new MyObject(this);
        obj1->setIntProp(18);
        MyObject *obj2 = new MyObject(this);
        QVERIFY(obj1->intProp() != obj2->intProp());
        new PropertyBinder(obj1, "intProp", obj2, "intProp");
        QCOMPARE(obj2->intProp(), 18);
    }
};

QTEST_MAIN(PropertyBinderTest)

#include "propertybindertest.moc"
