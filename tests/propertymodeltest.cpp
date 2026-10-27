/*
  propertymodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "testhelpers.h"
#include "baseprobetest.h"

#include <core/objectinstance.h>
#include <core/aggregatedpropertymodel.h>

#include "shared/propertytestobject.h"

#include <QAbstractItemModelTester>
#include <QDebug>
#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include <memory>
#include <vector>

using namespace GammaRay;
using namespace TestHelpers;

// an object with a child object, to build property trees of any depth
class ChainObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QObject *child READ child WRITE setChild NOTIFY childChanged)
public:
    explicit ChainObject(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    QObject *child() const
    {
        return m_child;
    }
    void setChild(QObject *child)
    {
        m_child = child;
        emit childChanged();
    }

signals:
    void childChanged();

private:
    QObject *m_child = nullptr;
};

// writing the flag also changes the child property of the owner, like when a sub-object is replaced
class FlagObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool flag READ flag WRITE setFlag NOTIFY flagChanged)
public:
    explicit FlagObject(ChainObject *owner)
        : m_owner(owner)
    {
    }

    bool flag() const
    {
        return m_flag;
    }
    void setFlag(bool flag)
    {
        m_flag = flag;
        emit flagChanged();
        m_owner->setChild(this);
    }

signals:
    void flagChanged();

private:
    ChainObject *m_owner;
    bool m_flag = false;
};

class PropertyModelTest : public BaseProbeTest
{
    Q_OBJECT
private slots:
    void initTestCase()
    {
        createProbe();
    }

    static void testPropertyModel()
    {
        PropertyTestObject obj;
        obj.setProperty("dynamicProperty", 5);

        AggregatedPropertyModel model;
        QAbstractItemModelTester modelTest(&model);
        model.setObject(&obj);

        QVERIFY(model.rowCount() > 9);
        auto dynRow = searchFixedIndex(&model, "dynamicProperty");
        QVERIFY(dynRow.isValid());
        QCOMPARE(dynRow.data(Qt::DisplayRole).toString(), QStringLiteral("dynamicProperty"));
        QVERIFY(dynRow.sibling(dynRow.row(), 1).flags() & Qt::ItemIsEditable);
        QCOMPARE(dynRow.sibling(dynRow.row(), 1).data(Qt::DisplayRole).toString(),
                 QStringLiteral("5"));
        QCOMPARE(dynRow.sibling(dynRow.row(), 1).data(Qt::EditRole), QVariant(5));

        auto qmoRow = searchFixedIndex(&model, "intProp");
        QVERIFY(qmoRow.isValid());
        QCOMPARE(qmoRow.data(Qt::DisplayRole).toString(), QStringLiteral("intProp"));
        auto qmoRow2 = qmoRow.sibling(qmoRow.row(), 1);
        QVERIFY(qmoRow2.flags() & Qt::ItemIsEditable);
        QCOMPARE(qmoRow2.data(Qt::DisplayRole).toString(), QStringLiteral("0"));
        QCOMPARE(qmoRow2.data(Qt::EditRole), QVariant(0));
        model.setData(qmoRow2, 12);
        QCOMPARE(obj.intProp(), 12);

        auto moRow = searchFixedIndex(&model, "thread");
        QVERIFY(moRow.isValid());
        QCOMPARE(moRow.data(Qt::DisplayRole).toString(), QStringLiteral("thread"));
        QVERIFY((moRow.sibling(moRow.row(), 1).flags() & Qt::ItemIsEditable) == 0);
        QVERIFY(!moRow.sibling(moRow.row(), 1).data(Qt::DisplayRole).toString().isEmpty());
    }

    static void testMetaObject()
    {
        AggregatedPropertyModel model;
        model.setObject(ObjectInstance(nullptr, &Gadget::staticMetaObject));
        QAbstractItemModelTester modelTest(&model);

        QCOMPARE(model.rowCount(), 1);
        auto qmoRow = searchFixedIndex(&model, "prop1");
        QVERIFY(qmoRow.isValid());
    }

    static void testChangeNotification()
    {
        ChangingPropertyObject obj;
        AggregatedPropertyModel model;
        QAbstractItemModelTester modelTest(&model);
        model.setObject(&obj);
        QVERIFY(model.rowCount() >= 4);

        QSignalSpy changeSpy(&model, &QAbstractItemModel::dataChanged);
        QVERIFY(changeSpy.isValid());
        QSignalSpy addSpy(&model, &QAbstractItemModel::rowsInserted);
        QVERIFY(addSpy.isValid());
        QSignalSpy removeSpy(&model, &QAbstractItemModel::rowsRemoved);
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

    // Replacing a sub-tree deletes all its adaptors, the model must not keep anything of the deeper
    // levels. New adaptors can get the addresses of the deleted ones, and must not end up with
    // their rows then (issue #1133).
    void testReplaceSubTree()
    {
        ChainObject root;
        AggregatedPropertyModel model;
        QAbstractItemModelTester modelTest(&model);
        model.setObject(&root);

        // every new chain has more dynamic properties than the previous one, so mixing up the
        // adaptors of two chains shows in the row counts
        std::vector<std::unique_ptr<ChainObject>> chains;
        for (int iteration = 0; iteration < 10; ++iteration) {
            const int depth = 4;
            std::vector<ChainObject *> chain;
            for (int level = 0; level < depth; ++level) {
                chains.push_back(std::make_unique<ChainObject>());
                auto obj = chains.back().get();
                for (int i = 0; i <= iteration; ++i)
                    obj->setProperty(QByteArray("dyn" + QByteArray::number(i)).constData(), i);
                if (!chain.empty())
                    chain.back()->setChild(obj);
                chain.push_back(obj);
            }
            root.setChild(chain.front());

            // expand the whole chain, which creates the adaptors
            QModelIndex parent;
            for (int level = 0; level < depth; ++level) {
                QModelIndex index;
                for (int row = 0; row < model.rowCount(parent); ++row) {
                    if (model.index(row, 0, parent).data().toString() == QLatin1String("child"))
                        index = model.index(row, 0, parent);
                }
                QVERIFY(index.isValid());
                AggregatedPropertyModel freshModel;
                freshModel.setObject(chain.at(level));
                QCOMPARE(model.rowCount(index), freshModel.rowCount());
                parent = index;
            }

            // a change deep down in the chain must find all of its rows
            chain.back()->setProperty("dyn0", -1);
        }
    }

    // the adaptors of removed rows must go away with them, and not report changes anymore
    static void testRemoveExpandedProperty()
    {
        QObject obj;
        QObject child;
        obj.setProperty("childObject", QVariant::fromValue<QObject *>(&child));

        AggregatedPropertyModel model;
        QAbstractItemModelTester modelTest(&model);
        model.setObject(&obj);
        const auto row = searchFixedIndex(&model, QStringLiteral("childObject"));
        QVERIFY(row.isValid());
        QVERIFY(model.rowCount(row) > 0); // creates the adaptor for child

        QSignalSpy removeSpy(&model, &QAbstractItemModel::rowsRemoved);
        obj.setProperty("childObject", QVariant());
        QCOMPARE(removeSpy.size(), 1);
        QVERIFY(!searchFixedIndex(&model, QStringLiteral("childObject")).isValid());

        QSignalSpy addSpy(&model, &QAbstractItemModel::rowsInserted);
        child.setProperty("dynamicProperty", 1);
        QCOMPARE(addSpy.size(), 0);
    }

    // the write can reload the sub-tree the written row is in
    void testCheckStateWriteReloadsParent()
    {
        ChainObject root;
        FlagObject flagObject(&root);
        root.setChild(&flagObject);

        AggregatedPropertyModel model;
        QAbstractItemModelTester modelTest(&model);
        model.setObject(&root);
        const auto childRow = searchFixedIndex(&model, QStringLiteral("child"));
        QVERIFY(childRow.isValid());
        QVERIFY(model.rowCount(childRow) > 0);

        QModelIndex flagValue;
        for (int row = 0; row < model.rowCount(childRow); ++row) {
            if (model.index(row, 0, childRow).data().toString() == QLatin1String("flag"))
                flagValue = model.index(row, 1, childRow);
        }
        QVERIFY(flagValue.isValid());
        QVERIFY(flagValue.flags() & Qt::ItemIsUserCheckable);
        QCOMPARE(flagValue.data(Qt::CheckStateRole).toInt(), int(Qt::Unchecked));

        QVERIFY(model.setData(flagValue, Qt::Checked, Qt::CheckStateRole));
        QVERIFY(flagObject.flag());
        QVERIFY(model.rowCount(childRow) > 0);
    }

    static void testGadgetRO()
    {
        PropertyTestObject obj;
        AggregatedPropertyModel model;
        model.setObject(ObjectInstance(&obj));

        auto idx = searchFixedIndex(&model, "gadgetReadOnly");
        QVERIFY(idx.isValid());
        QCOMPARE(model.rowCount(idx), 1);
        idx = model.index(0, 1, idx);
        QVERIFY((idx.flags() & Qt::ItemIsEditable) == 0);
    }

    static void testGadgetRW()
    {
        PropertyTestObject obj;
        AggregatedPropertyModel model;
        model.setObject(ObjectInstance(&obj));
        QAbstractItemModelTester modelTest(&model);

        auto idx = searchFixedIndex(&model, "gadget");
        QVERIFY(idx.isValid());
        QCOMPARE(model.rowCount(idx), 1);
        idx = model.index(0, 1, idx);
        QVERIFY(idx.flags() & Qt::ItemIsEditable);
        QVERIFY(model.setData(idx, 1554));
        QCOMPARE(obj.gadgetPointer()->prop1(), 1554);

        idx = searchFixedIndex(&model, "gadgetPointer");
        QVERIFY(idx.isValid());
        QCOMPARE(model.rowCount(idx), 1);
        idx = model.index(0, 1, idx);
        QVERIFY(idx.flags() & Qt::ItemIsEditable);
        QVERIFY(model.setData(idx, 1559));
        QCOMPARE(obj.gadgetPointer()->prop1(), 1559);
    }
};

QTEST_MAIN(PropertyModelTest)

#include "propertymodeltest.moc"
