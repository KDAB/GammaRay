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

private:
    int i = 0;

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
};

QTEST_MAIN(MethodModelTest)

#include "methodmodeltest.moc"
