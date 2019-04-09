/*
  propertyadaptortest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <core/metaobject.h>
#include <core/metaobjectrepository.h>

#include <shared/propertytestobject.h>

#include <QDebug>
#include <QtTest/qtest.h>
#include <QObject>
#include <QThread>
#include <QSignalSpy>
#include <QPen>

Q_DECLARE_METATYPE(QVector<int>)
Q_DECLARE_METATYPE(QPen *)

using namespace GammaRay;

class PropertyAdaptorTest : public QObject
{
    Q_OBJECT
private:
    void testProperty(PropertyAdaptor *adaptor, const char *name, const char *typeName,
                      const char *className, PropertyData::AccessFlags flags)
    {
        for (int i = 0; i < adaptor->count(); ++i) {
            auto prop = adaptor->propertyData(i);
            if (prop.name() != QLatin1String(name))
                continue;
            QCOMPARE(prop.typeName(), QString(typeName));
            QCOMPARE(prop.className(), QString(className));
            QCOMPARE(prop.accessFlags(), flags);
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

    int indexOfProperty(PropertyAdaptor *adaptor, const char *name)
    {
        for (int i = 0; i < adaptor->count(); ++i) {
            auto prop = adaptor->propertyData(i);
            if (prop.name() == QLatin1String(name))
                return i;
        }
        return -1;
    }

private slots:
    void initTestCase()
    {
        MetaObject *mo;
        MO_ADD_METAOBJECT0(QPen);
        MO_ADD_PROPERTY(QPen, color, setColor);
        MO_ADD_PROPERTY(QPen, width, setWidth);
    }

    void testQtGadget()
    {
        Gadget gadget;

        auto adaptor
            = PropertyAdaptorFactory::create(ObjectInstance(&gadget,
                                                            &Gadget::staticMetaObject), this);
        QVERIFY(adaptor);
        QCOMPARE(adaptor->count(), 1);
        verifyPropertyData(adaptor);
        testProperty(adaptor, "prop1", "int", "Gadget",
                     PropertyData::Writable | PropertyData::Resettable);
        QVERIFY(!adaptor->canAddProperty());

        QSignalSpy spy(adaptor, SIGNAL(propertyChanged(int,int)));
        QVERIFY(spy.isValid());

        QCOMPARE(adaptor->propertyData(0).value(), QVariant(42));
        adaptor->writeProperty(0, 23);
        QCOMPARE(adaptor->propertyData(0).value(), QVariant(23));
        QCOMPARE(spy.size(), 1);
        QCOMPARE(spy.at(0).at(0).toInt(), 0);
        QCOMPARE(spy.at(0).at(1).toInt(), 0);

        adaptor->resetProperty(0);
        QCOMPARE(adaptor->propertyData(0).value(), QVariant(5));
        QCOMPARE(spy.size(), 2);
    }

    void testROMetaObject()
    {
        PropertyTestObject obj;
        auto adaptor = PropertyAdaptorFactory::create(ObjectInstance(&obj, "QObject"), this);
        QVERIFY(adaptor);

        QCOMPARE(adaptor->count(), 4);
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
        QCOMPARE(adaptor->propertyData(idx).accessFlags(), PropertyData::Writable);
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

        QVERIFY(adaptor);
        QCOMPARE(adaptor->count(), 4);
        verifyPropertyData(adaptor);
        testProperty(adaptor, "0", "int", "QVector<int>", PropertyData::Readable);
        testProperty(adaptor, "3", "int", "QVector<int>", PropertyData::Readable);
        QVERIFY(!adaptor->canAddProperty());
    }

    void testAssociativeContainer()
    {
        QHash<QString, int> h;
        h["A"] = 2;
        h["B"] = 3;
        h["C"] = 5;

        auto adaptor = PropertyAdaptorFactory::create(ObjectInstance(QVariant::fromValue(h)), this);

        QVERIFY(adaptor);
        QCOMPARE(adaptor->count(), 3);
        verifyPropertyData(adaptor);
        testProperty(adaptor, "A", "int", "QHash<QString,int>", PropertyData::Readable);
        testProperty(adaptor, "C", "int", "QHash<QString,int>", PropertyData::Readable);
        QVERIFY(!adaptor->canAddProperty());
    }

    void testQtObject()
    {
        auto obj = new PropertyTestObject;
        obj->setProperty("dynamicProperty", 5);

        auto adaptor = PropertyAdaptorFactory::create(ObjectInstance(obj), this);
        QVERIFY(adaptor);

        QVERIFY(adaptor->count() > 9);
        verifyPropertyData(adaptor);

        testProperty(adaptor, "signalsBlocked", "bool", "QObject", PropertyData::Readable);
        testProperty(adaptor, "intProp", "int", "PropertyTestObject", PropertyData::Writable);
        testProperty(adaptor, "readOnlyProp", "int", "PropertyTestObject",
                     PropertyData::Resettable);
        testProperty(adaptor, "dynamicProperty", "int", "<dynamic>",
                     PropertyData::Writable | PropertyData::Deletable);

        QSignalSpy changeSpy(adaptor, SIGNAL(propertyChanged(int,int)));
        QVERIFY(changeSpy.isValid());

        auto propIdx = indexOfProperty(adaptor, "intProp");
        QVERIFY(propIdx >= 0);
        QCOMPARE(adaptor->propertyData(propIdx).revision(), 0);
        QCOMPARE(adaptor->propertyData(propIdx).notifySignal(), QLatin1String("void intPropChanged()"));
        QCOMPARE(adaptor->propertyData(propIdx).propertyFlags(), PropertyModel::Writable | PropertyModel::Designable | PropertyModel::Stored | PropertyModel::Scriptable);
        QCOMPARE(adaptor->propertyData(propIdx).value(), QVariant(0));
        adaptor->writeProperty(propIdx, 2);
        QCOMPARE(adaptor->propertyData(propIdx).value(), QVariant(2));
        QCOMPARE(changeSpy.size(), 1);
        QCOMPARE(changeSpy.at(0).at(0).toInt(), propIdx);
        QCOMPARE(changeSpy.at(0).at(1).toInt(), propIdx);

        obj->setIntProp(5);
        QCOMPARE(changeSpy.size(), 2);
        QCOMPARE(changeSpy.at(1).at(0).toInt(), propIdx);
        QCOMPARE(changeSpy.at(1).at(1).toInt(), propIdx);
        QCOMPARE(adaptor->propertyData(propIdx).value(), QVariant(5));

        propIdx = indexOfProperty(adaptor, "readOnlyProp");
        QVERIFY(propIdx >= 0);
        QCOMPARE(adaptor->propertyData(propIdx).revision(), 0);
        QCOMPARE(adaptor->propertyData(propIdx).notifySignal(), QString());
        QCOMPARE(adaptor->propertyData(propIdx).propertyFlags(), PropertyModel::Resetable | PropertyModel::Designable | PropertyModel::Stored | PropertyModel::Scriptable);
        adaptor->resetProperty(propIdx);
        QCOMPARE(obj->intProp(), 5);
        QVERIFY(changeSpy.size() >= 3);

        propIdx = indexOfProperty(adaptor, "dynamicProperty");
        QVERIFY(propIdx >= 0);
        changeSpy.clear();
        adaptor->writeProperty(propIdx, 12);
        QCOMPARE(changeSpy.size(), 1);
        QCOMPARE(changeSpy.at(0).at(0).toInt(), propIdx);
        QCOMPARE(changeSpy.at(0).at(1).toInt(), propIdx);
        QCOMPARE(obj->property("dynamicProperty").toInt(), 12);

        QSignalSpy addSpy(adaptor, SIGNAL(propertyAdded(int,int)));
        QVERIFY(addSpy.isValid());
        QSignalSpy removeSpy(adaptor, SIGNAL(propertyRemoved(int,int)));
        QVERIFY(removeSpy.isValid());

        QVERIFY(adaptor->canAddProperty());
        PropertyData newProp;
        newProp.setName(QStringLiteral("newProperty"));
        newProp.setValue(QStringLiteral("value"));
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

        QSignalSpy invalidatedSpy(adaptor, SIGNAL(objectInvalidated()));
        QVERIFY(invalidatedSpy.isValid());
        delete obj;
        QVERIFY(!invalidatedSpy.isEmpty());
    }

    void testQtMetaObject()
    {
        auto adaptor
            = PropertyAdaptorFactory::create(ObjectInstance(nullptr,
                                                            &PropertyTestObject::staticMetaObject),
                                             this);
        QVERIFY(adaptor);
        QVERIFY(adaptor->count() >= 5);
        verifyPropertyData(adaptor);
    }

    void testVariant()
    {
        QPen pen(Qt::red);
        auto valuePen = QVariant::fromValue(pen);
        auto pointerPen = QVariant::fromValue(&pen);

        auto adaptor = PropertyAdaptorFactory::create(ObjectInstance(valuePen), this);
        QVERIFY(adaptor);
        QVERIFY(adaptor->count() >= 2);
        verifyPropertyData(adaptor);

        adaptor = PropertyAdaptorFactory::create(ObjectInstance(pointerPen), this);
        QVERIFY(adaptor);
        QVERIFY(adaptor->count() >= 2);
        verifyPropertyData(adaptor);
    }
};

QTEST_MAIN(PropertyAdaptorTest)

#include "propertyadaptortest.moc"
