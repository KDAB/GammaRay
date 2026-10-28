/*
  methodmodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "baseprobetest.h"
#include "testhelpers.h"

#include "common/objectbroker.h"
#include <core/objectmethodmodel.h>
#include <ui/tools/objectinspector/clientmethodmodel.h>
#include <common/tools/objectinspector/methodmodel.h>
#include <common/tools/objectinspector/methodsextensioninterface.h>
#include <core/tools/objectinspector/methodsextension.h>

#include <QAbstractItemModelTester>
#include <QDebug>
#include <QItemSelectionModel>

#ifndef Q_MOC_RUN
#define MY_TAG
#endif

using namespace GammaRay;
using namespace TestHelpers;

class MethodModelTest : public BaseProbeTest
{
    Q_OBJECT
public slots:
    MY_TAG void taggedSlot()
    {
    }

    Q_REVISION(0, 147)
    void revisionedSlot()
    {
    }

    void bumpI()
    {
        // printf("Bump Idx Called\n");
        i++;
    }

    void takeArgs(const QString &s, int n, double d)
    {
        takenArgs = QVariantList({ s, n, d });
    }

private:
    int i = 0;
    QVariantList takenArgs;

private slots:
    void modelTest()
    {
        createProbe();

        ObjectMethodModel srcModel;
        ClientMethodModel model;
        model.setSourceModel(&srcModel);

        QAbstractItemModelTester modelTest(&model);

        srcModel.setMetaObject(&QObject::staticMetaObject);
        srcModel.setMetaObject(&staticMetaObject);
        srcModel.setMetaObject(nullptr);
    }

    static void testModel()
    {
        ObjectMethodModel srcModel;
        ClientMethodModel model;
        model.setSourceModel(&srcModel);

        srcModel.setMetaObject(&staticMetaObject);

        QVERIFY(model.rowCount() > 0);

        auto idx = searchContainsIndex(&model, "deleteLater()");
        QVERIFY(idx.isValid());
        QVERIFY(idx.data(ObjectMethodModelRole::MethodSignature).toString().startsWith(QLatin1String("deleteLater")));
        QCOMPARE(idx.sibling(idx.row(), 1).data().toString(), QLatin1String("Slot"));
        QVERIFY(idx.sibling(idx.row(), 1).data(ObjectMethodModelRole::MethodSignature).isNull());
        QCOMPARE(idx.sibling(idx.row(), 2).data().toString(), QLatin1String("Public"));
        QCOMPARE(idx.sibling(idx.row(), 3).data().toString(), QLatin1String("QObject"));

        srcModel.setMetaObject(nullptr);
        QCOMPARE(model.rowCount(), 0);
    }

    static void testToolTip_data()
    {
        QTest::addColumn<QString>("name", nullptr);
        QTest::addColumn<QString>("toolTip", nullptr);

        QTest::newRow("tagged") << "taggedSlot"
                                << "MY_TAG";
        QTest::newRow("revision") << "revisionedSlot"
                                  << "147";
    }

    static void testToolTip()
    {
        QFETCH(QString, name);
        QFETCH(QString, toolTip);

        ObjectMethodModel srcModel;
        ClientMethodModel model;
        model.setSourceModel(&srcModel);
        srcModel.setMetaObject(&staticMetaObject);

        auto idx = searchContainsIndex(&model, name);
        QVERIFY(idx.isValid());
        QCOMPARE(model.columnCount(), 4);
        for (int i = 0; i < model.columnCount(); ++i) {
            QVERIFY(idx.sibling(idx.row(), i).data(Qt::ToolTipRole).toString().contains(toolTip));
        }
    }

    void testInvokeMethod()
    {
        auto iface = ObjectBroker::object<MethodsExtensionInterface *>("com.kdab.GammaRay.ObjectInspector.methodsExtension");
        auto model = ObjectBroker::model("com.kdab.GammaRay.ObjectInspector.methods");
        auto selModel = ObjectBroker::selectionModel(model);

        static_cast<MethodsExtension *>(iface)->setQObject(this);

        QVERIFY(model->rowCount() > 0);

        auto idx = searchContainsIndex(model, "bumpI()");
        QVERIFY(idx.isValid());

        selModel->select(idx, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect);

        iface->activateMethod();
        iface->invokeMethod(Qt::AutoConnection);

        QCOMPARE(i, 1);
    }

    // the arguments start with a default value of their type, which is also what gets passed
    void testArgumentDefaults()
    {
        auto iface = ObjectBroker::object<MethodsExtensionInterface *>("com.kdab.GammaRay.ObjectInspector.methodsExtension");
        auto model = ObjectBroker::model("com.kdab.GammaRay.ObjectInspector.methods");
        auto argModel = ObjectBroker::model("com.kdab.GammaRay.ObjectInspector.methodArguments");
        QVERIFY(argModel);
        auto selModel = ObjectBroker::selectionModel(model);

        static_cast<MethodsExtension *>(iface)->setQObject(this);
        auto idx = searchContainsIndex(model, "takeArgs(");
        QVERIFY(idx.isValid());
        selModel->select(idx, QItemSelectionModel::Rows | QItemSelectionModel::ClearAndSelect);
        iface->activateMethod();

        QCOMPARE(argModel->rowCount(), 3);
        QCOMPARE(argModel->index(0, 1).data(Qt::EditRole).metaType(), QMetaType::fromType<QString>());
        QCOMPARE(argModel->index(1, 1).data(Qt::EditRole).metaType(), QMetaType::fromType<int>());
        QCOMPARE(argModel->index(1, 1).data(Qt::EditRole).toInt(), 0);
        QCOMPARE(argModel->index(2, 1).data(Qt::EditRole).metaType(), QMetaType::fromType<double>());

        iface->invokeMethod(Qt::DirectConnection);
        QCOMPARE(takenArgs, QVariantList({ QString(), 0, 0.0 }));
    }
};

QTEST_MAIN(MethodModelTest)

#include "methodmodeltest.moc"
