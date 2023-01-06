/*
  methodmodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "baseprobetest.h"
#include "testhelpers.h"

#include <core/objectmethodmodel.h>
#include <ui/tools/objectinspector/clientmethodmodel.h>
#include <common/tools/objectinspector/methodmodel.h>

#include <3rdparty/qt/modeltest.h>

#include <QDebug>

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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Q_REVISION(147)
    void revisionedSlot()
    {
    }
#else
    Q_REVISION(0, 147)
    void revisionedSlot()
    {
    }
#endif

private slots:
    void modelTest()
    {
        createProbe();

        ObjectMethodModel srcModel;
        ClientMethodModel model;
        model.setSourceModel(&srcModel);

        ModelTest modelTest(&model);

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
};

QTEST_MAIN(MethodModelTest)

#include "methodmodeltest.moc"
