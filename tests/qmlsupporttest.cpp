/*
  qmlsupporttest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <plugins/qmlsupport/qmllistpropertyadaptor.h>
#include <plugins/qmlsupport/qmlattachedpropertyadaptor.h>
#include <plugins/qmlsupport/qjsvaluepropertyadaptor.h>
#include <plugins/qmlsupport/qmlcontextpropertyadaptor.h>

#include <core/propertyadaptor.h>
#include <core/propertyadaptorfactory.h>
#include <core/objectinstance.h>
#include <core/propertydata.h>

#include <QQmlComponent>
#include <QQmlContext>
#include <QQmlEngine>

#include <QDebug>
#include <QObject>
#include <QSignalSpy>
#include <QThread>
#include <QTest>

using namespace GammaRay;

class QmlSupportTest : public QObject
{
    Q_OBJECT
private:
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
        PropertyAdaptorFactory::registerFactory(QmlListPropertyAdaptorFactory::instance());
        PropertyAdaptorFactory::registerFactory(QmlAttachedPropertyAdaptorFactory::instance());
        PropertyAdaptorFactory::registerFactory(QJSValuePropertyAdaptorFactory::instance());
        PropertyAdaptorFactory::registerFactory(QmlContextPropertyAdaptorFactory::instance());
    }

    void testQmlListProperty()
    {
        QQmlEngine engine;
        QQmlComponent component(&engine);
        component.setData("import QtQuick 2.0\nRectangle { Text { text: \"Hello world!\" } }",
                          QUrl());
        auto obj = component.create();
        QVERIFY(obj);

        auto adaptor = PropertyAdaptorFactory::create(obj, this);
        QVERIFY(adaptor);
        QVERIFY(adaptor->count() > 20);

        auto idx = indexOfProperty(adaptor, "data");
        QVERIFY(idx >= 0);
        auto pd = adaptor->propertyData(idx);
        QCOMPARE(pd.className(), QStringLiteral("QQuickItem"));

        auto listAdaptor = PropertyAdaptorFactory::create(pd.value(), this);
        QVERIFY(listAdaptor);
        QCOMPARE(listAdaptor->count(), 1);

        auto data = listAdaptor->propertyData(0);
        QVERIFY(!data.name().isEmpty());
        QVERIFY(data.value().canConvert<QObject *>());
        QVERIFY(!data.typeName().isEmpty());
        QVERIFY(!data.className().isEmpty());

        delete obj;
    }

    void testAttachedProperty()
    {
        QQmlEngine engine;
        QQmlComponent component(&engine);
        component.setData("import QtQuick 2.0\nRectangle { Keys.enabled: true }", QUrl());
        auto obj = component.create();
        QVERIFY(obj);

        auto adaptor = PropertyAdaptorFactory::create(obj, this);
        QVERIFY(adaptor);
        QVERIFY(adaptor->count() > 20);

        auto idx = indexOfProperty(adaptor, "Keys");
        QVERIFY(idx >= 0);

        auto data = adaptor->propertyData(idx);
        QCOMPARE(data.name(), QStringLiteral("Keys"));
        QVERIFY(!data.typeName().isEmpty());
        QVERIFY(data.value().isValid());
        QVERIFY(data.value().canConvert<QObject *>());
        QVERIFY(!data.className().isEmpty());

        delete obj;
    }

    void testJSValue()
    {
        QQmlEngine engine;
        QQmlComponent component(&engine);
        component.setData(
            "import QtQuick 2.0\nRectangle { property var a1: []; property var a2: [\"hello\", \"world\"] }",
            QUrl());
        auto obj = component.create();
        QVERIFY(obj);

        auto adaptor = PropertyAdaptorFactory::create(obj, this);
        QVERIFY(adaptor);

        auto idx = indexOfProperty(adaptor, "a1");
        QVERIFY(idx >= 0);

        auto data = adaptor->propertyData(idx);
        auto jsValueAdaptor = PropertyAdaptorFactory::create(data.value(), this);
        QVERIFY(jsValueAdaptor);
        QCOMPARE(jsValueAdaptor->count(), 0);

        idx = indexOfProperty(adaptor, "a2");
        QVERIFY(idx >= 0);

        data = adaptor->propertyData(idx);
        jsValueAdaptor = PropertyAdaptorFactory::create(data.value(), this);
        QVERIFY(jsValueAdaptor);
        QCOMPARE(jsValueAdaptor->count(), 2);
        data = jsValueAdaptor->propertyData(1);
        QCOMPARE(data.name(), QStringLiteral("1"));
        QCOMPARE(data.value(), QVariant("world"));

        delete obj;
    }

    void testContextProperty()
    {
        QQmlEngine engine;
        engine.rootContext()->setContextProperty("myContextProp", 42);

        auto adaptor = PropertyAdaptorFactory::create(engine.rootContext(), this);
        QVERIFY(adaptor);

        auto idx = indexOfProperty(adaptor, "myContextProp");
        QVERIFY(idx >= 0);

        auto data = adaptor->propertyData(idx);
        QCOMPARE(data.name(), QStringLiteral("myContextProp"));
        QCOMPARE(data.value().toInt(), 42);

        adaptor->writeProperty(idx, 23);
        QCOMPARE(engine.rootContext()->contextProperty("myContextProp").toInt(), 23);
    }
};

QTEST_MAIN(QmlSupportTest)

#include "qmlsupporttest.moc"
