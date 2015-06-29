/*
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

#include <core/metaobjectrepository.h>
#include <core/metaobject.h>

#include <QDebug>
#include <QtTest/qtest.h>
#include <QObject>
#include <QThread>

Q_DECLARE_METATYPE(QThread::Priority)

using namespace GammaRay;

class MetaObjectTest : public QObject
{
    Q_OBJECT
private slots:
    void testMetaObject()
    {
      QVERIFY(MetaObjectRepository::instance()->hasMetaObject("QThread"));
      auto *mo = MetaObjectRepository::instance()->metaObject("QThread");

      QVERIFY(mo);
      QCOMPARE(mo->className(), QString("QThread"));
      QVERIFY(mo->inherits("QObject"));

      auto *superMo = mo->superClass(0);
      QVERIFY(superMo);
      QCOMPARE(superMo->className(), QString("QObject"));

      QVERIFY(!mo->superClass(1));
      QVERIFY(!superMo->superClass(0));
    }

    void testMemberProperty()
    {
      auto *mo = MetaObjectRepository::instance()->metaObject("QThread");
      QVERIFY(mo->propertyCount() >= 7); // depends on Qt version

      MetaProperty *prop = 0;
      for (int i = 0; i < mo->propertyCount(); ++i) {
        prop = mo->propertyAt(i);
        QVERIFY(prop);
        if (prop->name() == "priority")
          break;
      }

      QVERIFY(prop);
      QCOMPARE(prop->name(), QString("priority"));
      QCOMPARE(prop->typeName(), QString("QThread::Priority"));

      QThread t;
      QCOMPARE(prop->value(&t).value<QThread::Priority>(), t.priority());
      QCOMPARE(prop->isReadOnly(), false);
    }

    void testStaticProperty()
    {
      auto *mo = MetaObjectRepository::instance()->metaObject("QCoreApplication");
      QVERIFY(mo);
      QVERIFY(mo->propertyCount() >= 8); // depends on Qt version

      MetaProperty *prop = 0;
      for (int i = 0; i < mo->propertyCount(); ++i) {
        prop = mo->propertyAt(i);
        QVERIFY(prop);
        if (prop->name() == "libraryPaths")
          break;
      }

      QVERIFY(prop);
      QCOMPARE(prop->name(), QString("libraryPaths"));
      QCOMPARE(prop->typeName(), QString("QStringList"));
      QCOMPARE(prop->isReadOnly(), true);
      QCOMPARE(prop->value(0).toStringList(), QCoreApplication::libraryPaths());
    }

};

QTEST_MAIN(MetaObjectTest)

#include "metaobjecttest.moc"
