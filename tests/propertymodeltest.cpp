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

#include <3rdparty/qt/modeltest.h>

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
        MyObject obj;
        obj.setProperty("dynamicProperty", 5);

        AggregatedPropertyModel model;
        ModelTest modelTest(&model);
        model.setObject(&obj);

        QVERIFY(model.rowCount() > 7);
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

};

QTEST_MAIN(PropertyModelTest)

#include "propertymodeltest.moc"
