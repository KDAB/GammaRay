/*
  qmlsupporttest.cpp

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

#include <plugins/qmlsupport/qmllistpropertyadaptor.h>
#include <plugins/qmlsupport/qmlattachedpropertyadaptor.h>

#include <core/propertyadaptor.h>
#include <core/propertyadaptorfactory.h>
#include <core/objectinstance.h>
#include <core/propertydata.h>

#include <QQmlComponent>
#include <QQmlEngine>

#include <QDebug>
#include <QtTest/qtest.h>
#include <QObject>
#include <QThread>
#include <QSignalSpy>

using namespace GammaRay;

class QmlSupportTest : public QObject
{
    Q_OBJECT
private:
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
    void initTestCase()
    {
        PropertyAdaptorFactory::registerFactory(QmlListPropertyAdaptorFactory::instance());
        PropertyAdaptorFactory::registerFactory(QmlAttachedPropertyAdaptorFactory::instance());
    }

    void testQmlListProperty()
    {
        QQmlEngine engine;
        QQmlComponent component(&engine);
        component.setData("import QtQuick 2.0\nRectangle { Text { text: \"Hello world!\" } }", QUrl());
        auto obj = component.create();
        QVERIFY(obj);

        auto adaptor = PropertyAdaptorFactory::create(obj, this);
        QVERIFY(adaptor);
        QVERIFY(adaptor->count() > 20);

        auto idx = indexOfProperty(adaptor, "data");
        QVERIFY(idx >= 0);
        auto pd = adaptor->propertyData(idx);
        QCOMPARE(pd.className(), QString("QQuickItem"));

        auto listAdaptor = PropertyAdaptorFactory::create(pd.value(), this);
        QVERIFY(listAdaptor);
        QCOMPARE(listAdaptor->count(), 1);

        auto data = listAdaptor->propertyData(0);
        QVERIFY(!data.name().isEmpty());
        QVERIFY(data.value().canConvert<QObject*>());
        QVERIFY(!data.typeName().isEmpty());
        QVERIFY(!data.className().isEmpty());
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
        QCOMPARE(data.name(), QString("Keys"));
        QVERIFY(!data.typeName().isEmpty());
        QVERIFY(data.value().isValid());
        QVERIFY(data.value().canConvert<QObject*>());
        QVERIFY(!data.className().isEmpty());
    }
};

QTEST_MAIN(QmlSupportTest)

#include "qmlsupporttest.moc"
