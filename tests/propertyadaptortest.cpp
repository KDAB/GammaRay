/*
  propertyadaptortest.cpp

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

#include <core/propertyadaptor.h>
#include <core/propertyadaptorfactory.h>
#include <core/objectinstance.h>
#include <core/propertydata.h>

#include <QDebug>
#include <QtTest/qtest.h>
#include <QObject>
#include <QThread>
#include <QSignalSpy>

class MyGadget
{
    Q_GADGET
    Q_PROPERTY(int prop1 READ prop1 WRITE setProp1 RESET resetProp1)

public:
    MyGadget() : m_prop1(42) {}
    int prop1() const { return m_prop1; }
    void setProp1(int v) { m_prop1 = v; }
    void resetProp1() { m_prop1 = 5; }
private:
    int m_prop1;
};

Q_DECLARE_METATYPE(MyGadget)

class MyObject : public QObject
{
    Q_PROPERTY(int intProp READ intProp WRITE setIntProp NOTIFY intPropChanged)
    Q_PROPERTY(int readOnlyProp READ intProp RESET resetIntProp)
    Q_PROPERTY(MyGadget gadget READ gadget)
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
    void resetIntProp()
    {
        setIntProp(5);
    }

    MyGadget gadget() const { return MyGadget(); }

signals:
    void intPropChanged();

private:
    int p1;
};

Q_DECLARE_METATYPE(QVector<int>)
#if QT_VERSION < QT_VERSION_CHECK(5, 2, 0)
typedef QHash<QString, int> StringIntHash;
Q_DECLARE_METATYPE(StringIntHash)
#endif

using namespace GammaRay;

class PropertyAdaptorTest : public QObject
{
    Q_OBJECT
private:
    void testProperty(PropertyAdaptor *adaptor, const char* name, const char* typeName, const char* className, PropertyData::Flags flags)
    {
        for (int i = 0; i < adaptor->count(); ++i) {
            auto prop = adaptor->propertyData(i);
            if (prop.name() != QLatin1String(name))
                continue;
            QCOMPARE(prop.typeName(), QString(typeName));
            QCOMPARE(prop.className(), QString(className));
            QCOMPARE(prop.flags(), flags);
            return;
        }
        qDebug() << name;
        QVERIFY(!"property not found");
    }

    void verifyPropertyData(PropertyAdaptor *adaptor)
    {
        for (int i = 0; i < adaptor->count(); ++i) {
            auto data = adaptor->propertyData(i);
            QVERIFY(!data.name().isEmpty());
            QVERIFY(!data.typeName().isEmpty());
            QVERIFY(!data.className().isEmpty());
        }
    }

    int indexOfProperty(PropertyAdaptor *adaptor, const char* name)
    {
        for (int i = 0; i < adaptor->count(); ++i) {
            auto prop = adaptor->propertyData(i);
            if (prop.name() == QLatin1String(name))
                return i;
        }
        return -1;
    }

private slots:
    void testQtGadget()
    {
        MyGadget gadget;

        auto adaptor = PropertyAdaptorFactory::create(ObjectInstance(&gadget, &MyGadget::staticMetaObject), this);
        QVERIFY(adaptor);
        QCOMPARE(adaptor->count(), 1);
        verifyPropertyData(adaptor);
        testProperty(adaptor, "prop1", "int", "MyGadget", PropertyData::Writable | PropertyData::Resettable);
        QVERIFY(!adaptor->canAddProperty());

        QSignalSpy spy(adaptor, SIGNAL(propertyChanged(int,int)));
        QVERIFY(spy.isValid());

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        QCOMPARE(adaptor->propertyData(0).value(), QVariant(42));
        adaptor->writeProperty(0, 23);
        QCOMPARE(adaptor->propertyData(0).value(), QVariant(23));
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(0).toInt(), 0);
        QCOMPARE(spy.at(0).at(1).toInt(), 0);

        adaptor->resetProperty(0);
        QCOMPARE(adaptor->propertyData(0).value(), QVariant(5));
        QCOMPARE(spy.size(), 2);
#endif
    }

    void testROMetaObject()
    {
        MyObject obj;
        auto adaptor = PropertyAdaptorFactory::create(ObjectInstance(&obj, "QObject"), this);
        QVERIFY(adaptor);

        QCOMPARE(adaptor->count(), 3);
        verifyPropertyData(adaptor);
        testProperty(adaptor, "parent", "QObject*", "QObject", PropertyData::Readable);
        testProperty(adaptor, "thread", "QThread*", "QObject", PropertyData::Readable);
        QVERIFY(!adaptor->canAddProperty());
    }

    void testRWMetaObject()
    {
        QThread obj;
        auto adaptor = PropertyAdaptorFactory::create(&obj, this);
        QVERIFY(adaptor);

        QVERIFY(adaptor->count() > 3);
        verifyPropertyData(adaptor);

        QSignalSpy spy(adaptor, SIGNAL(propertyChanged(int,int)));
        QVERIFY(spy.isValid());

        auto idx = indexOfProperty(adaptor, "priority");
        QCOMPARE(adaptor->propertyData(idx).flags(), PropertyData::Writable);
        adaptor->writeProperty(idx, QThread::LowPriority);
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(0).toInt(), idx);
        QCOMPARE(spy.at(0).at(1).toInt(), idx);
        adaptor->writeProperty(idx, QThread::NormalPriority);
        QCOMPARE(spy.size(), 2);
        QCOMPARE(spy.at(1).at(0).toInt(), idx);
        QCOMPARE(spy.at(1).at(1).toInt(), idx);
    }

    void testSequentialContainer()
    {
        auto v = QVector<int>() << 2 << 3 << 5 << 12;
        auto adaptor = PropertyAdaptorFactory::create(ObjectInstance(QVariant::fromValue(v)), this);

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
        QVERIFY(adaptor);
        QCOMPARE(adaptor->count(), 4);
        verifyPropertyData(adaptor);
        testProperty(adaptor, "0", "int", "QVector<int>", PropertyData::Readable);
        testProperty(adaptor, "3", "int", "QVector<int>", PropertyData::Readable);
        QVERIFY(!adaptor->canAddProperty());
#endif
    }

    void testAssociativeContainer()
    {
        QHash<QString, int> h;
        h["A"] = 2;
        h["B"] = 3;
        h["C"] = 5;

        auto adaptor = PropertyAdaptorFactory::create(ObjectInstance(QVariant::fromValue(h)), this);

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
        QVERIFY(adaptor);
        QCOMPARE(adaptor->count(), 3);
        verifyPropertyData(adaptor);
        testProperty(adaptor, "A", "int", "QHash<QString,int>", PropertyData::Readable);
        testProperty(adaptor, "C", "int", "QHash<QString,int>", PropertyData::Readable);
        QVERIFY(!adaptor->canAddProperty());
#endif
    }

    void testQtObject()
    {
        MyObject obj;
        obj.setProperty("dynamicProperty", 5);

        auto adaptor = PropertyAdaptorFactory::create(ObjectInstance(&obj), this);
        QVERIFY(adaptor);

        QVERIFY(adaptor->count() > 7);
        verifyPropertyData(adaptor);

        testProperty(adaptor, "signalsBlocked", "bool", "QObject", PropertyData::Readable);
        testProperty(adaptor, "intProp", "int", "MyObject", PropertyData::Writable);
        testProperty(adaptor, "readOnlyProp", "int", "MyObject", PropertyData::Resettable);
        testProperty(adaptor, "dynamicProperty", "int", "<dynamic>", PropertyData::Writable | PropertyData::Deletable);

        QSignalSpy changeSpy(adaptor, SIGNAL(propertyChanged(int,int)));
        QVERIFY(changeSpy.isValid());

        auto propIdx = indexOfProperty(adaptor, "intProp");
        QVERIFY(propIdx >= 0);
        QVERIFY(!adaptor->propertyData(propIdx).details().isEmpty());
        QCOMPARE(adaptor->propertyData(propIdx).value(), QVariant(0));
        adaptor->writeProperty(propIdx, 2);
        QCOMPARE(adaptor->propertyData(propIdx).value(), QVariant(2));
        QCOMPARE(changeSpy.size(), 1);
        QCOMPARE(changeSpy.at(0).at(0).toInt(), propIdx);
        QCOMPARE(changeSpy.at(0).at(1).toInt(), propIdx);

        obj.setIntProp(5);
        QCOMPARE(changeSpy.size(), 2);
        QCOMPARE(changeSpy.at(1).at(0).toInt(), propIdx);
        QCOMPARE(changeSpy.at(1).at(1).toInt(), propIdx);
        QCOMPARE(adaptor->propertyData(propIdx).value(), QVariant(5));

        propIdx = indexOfProperty(adaptor, "readOnlyProp");
        QVERIFY(propIdx >= 0);
        QVERIFY(!adaptor->propertyData(propIdx).details().isEmpty());
        adaptor->resetProperty(propIdx);
        QCOMPARE(obj.intProp(), 5);
        QVERIFY(changeSpy.size() >= 3);

        propIdx = indexOfProperty(adaptor, "dynamicProperty");
        QVERIFY(propIdx >= 0);
        changeSpy.clear();
        adaptor->writeProperty(propIdx, 12);
        QCOMPARE(changeSpy.size(), 1);
        QCOMPARE(changeSpy.at(0).at(0).toInt(), propIdx);
        QCOMPARE(changeSpy.at(0).at(1).toInt(), propIdx);
        QCOMPARE(obj.property("dynamicProperty").toInt(), 12);

        QSignalSpy addSpy(adaptor, SIGNAL(propertyAdded(int,int)));
        QVERIFY(addSpy.isValid());
        QSignalSpy removeSpy(adaptor, SIGNAL(propertyRemoved(int,int)));
        QVERIFY(removeSpy.isValid());

        QVERIFY(adaptor->canAddProperty());
        PropertyData newProp;
        newProp.setName("newProperty");
        newProp.setValue(QString("value"));
        auto oldPropCount = adaptor->count();
        adaptor->addProperty(newProp);
        QCOMPARE(oldPropCount + 1, adaptor->count());
        QCOMPARE(addSpy.size(), 1);
        propIdx = indexOfProperty(adaptor, "newProperty");
        QVERIFY(propIdx >= 0);

        adaptor->writeProperty(propIdx, QVariant());
        QCOMPARE(oldPropCount, adaptor->count());
        QCOMPARE(removeSpy.size(), 1);
        QCOMPARE(addSpy.size(), 1);
    }
};

QTEST_MAIN(PropertyAdaptorTest)

#include "propertyadaptortest.moc"
