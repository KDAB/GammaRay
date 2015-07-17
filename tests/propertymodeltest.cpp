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

#include <core/objectinstance.h>
#include <core/aggregatedpropertymodel.h>

#include "shared/propertytestobject.h"

#include <3rdparty/qt/modeltest.h>

#include <QDebug>
#include <QtTest/qtest.h>
#include <QObject>
#include <QThread>
#include <QSignalSpy>

using namespace GammaRay;

class PropertyModelTest : public QObject
{
    Q_OBJECT
private:
    QModelIndex findRowByName(QAbstractItemModel* model, const char* name)
    {
        for (int i = 0; i < model->rowCount(); ++i) {
            auto index = model->index(i, 0);
            if (index.data(Qt::DisplayRole).toString() == QLatin1String(name))
                return index;
        }
        return QModelIndex();
    }
private slots:
    void testPropertyModel()
    {
        PropertyTestObject obj;
        obj.setProperty("dynamicProperty", 5);

        AggregatedPropertyModel model;
        ModelTest modelTest(&model);
        model.setObject(&obj);

        QVERIFY(model.rowCount() > 9);
        auto dynRow = findRowByName(&model, "dynamicProperty");
        QVERIFY(dynRow.isValid());
        QCOMPARE(dynRow.data(Qt::DisplayRole).toString(), QString("dynamicProperty"));
        QVERIFY(dynRow.sibling(dynRow.row(), 1).flags() & Qt::ItemIsEditable);
        QCOMPARE(dynRow.sibling(dynRow.row(), 1).data(Qt::DisplayRole).toString(), QString("5"));
        QCOMPARE(dynRow.sibling(dynRow.row(), 1).data(Qt::EditRole), QVariant(5));

        auto qmoRow = findRowByName(&model, "intProp");
        QVERIFY(qmoRow.isValid());
        QCOMPARE(qmoRow.data(Qt::DisplayRole).toString(), QString("intProp"));
        auto qmoRow2 = qmoRow.sibling(qmoRow.row(), 1);
        QVERIFY(qmoRow2.flags() & Qt::ItemIsEditable);
        QCOMPARE(qmoRow2.data(Qt::DisplayRole).toString(), QString("0"));
        QCOMPARE(qmoRow2.data(Qt::EditRole), QVariant(0));
        model.setData(qmoRow2, 12);
        QCOMPARE(obj.intProp(), 12);

        auto moRow = findRowByName(&model, "thread");
        QVERIFY(moRow.isValid());
        QCOMPARE(moRow.data(Qt::DisplayRole).toString(), QString("thread"));
        QVERIFY((moRow.sibling(moRow.row(), 1).flags() & Qt::ItemIsEditable) == 0);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        QVERIFY(!moRow.sibling(moRow.row(), 1).data(Qt::DisplayRole).toString().isEmpty());
#endif
    }

    void testChangeNotification()
    {
        ChangingPropertyObject obj;
        AggregatedPropertyModel model;
//         ModelTest modelTest(&model);
        model.setObject(&obj);
        QVERIFY(model.rowCount() >= 4);

        QSignalSpy changeSpy(&model, SIGNAL(dataChanged(QModelIndex,QModelIndex)));
        QVERIFY(changeSpy.isValid());
        QSignalSpy addSpy(&model, SIGNAL(rowsInserted(QModelIndex,int,int)));
        QVERIFY(addSpy.isValid());
        QSignalSpy removeSpy(&model, SIGNAL(rowsRemoved(QModelIndex,int,int)));
        QVERIFY(removeSpy.isValid());

        obj.changeProperties();

        QCOMPARE(changeSpy.size(), 1);
        QCOMPARE(addSpy.size(), 1);

        obj.changeProperties();
        QCOMPARE(changeSpy.size(), 3);

        obj.setProperty("dynamicChangingProperty", QVariant());
        QCOMPARE(changeSpy.size(), 3);
        QCOMPARE(addSpy.size(), 1);
        QCOMPARE(removeSpy.size(), 1);
    }

};

QTEST_MAIN(PropertyModelTest)

#include "propertymodeltest.moc"
