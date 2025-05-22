/*
  metatypemodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "testhelpers.h"

#include <config-gammaray.h>

#include <core/tools/metatypebrowser/metatypesmodel.h>
#include <ui/tools/metatypebrowser/metatypesclientmodel.h>
#include <common/tools/metatypebrowser/metatyperoles.h>

#include <common/objectid.h>

#include <QAbstractItemModelTester>
#include <QDebug>
#include <QObject>
#include <QSignalSpy>
#include <QThread>
#include <QTest>

using namespace GammaRay;
using namespace TestHelpers;

class MetaTypeModelTest : public QObject
{
    Q_OBJECT
private slots:
    static void testModel()
    {
        MetaTypesModel srcModel;
        MetaTypesClientModel model;
        model.setSourceModel(&srcModel);
        QAbstractItemModelTester modelTest(&model);
        QVERIFY(model.rowCount() > 0);
        QSignalSpy resetSpy(&model, &QAbstractItemModel::modelReset);
        QVERIFY(resetSpy.isValid());

        auto idx = searchFixedIndex(&model, "QObject*");
        QVERIFY(idx.isValid());
        QVERIFY(!idx.data(MetaTypeRoles::MetaObjectIdRole).value<ObjectId>().isNull());

        idx = searchFixedIndex(&model, "int");
        QVERIFY(idx.isValid());
        QVERIFY(idx.data(MetaTypeRoles::MetaObjectIdRole).isNull());

        idx = searchFixedIndex(&model, "QThread*");
        QVERIFY(!idx.isValid());
        qRegisterMetaType<QThread *>();
        srcModel.scanMetaTypes();
        idx = searchFixedIndex(&model, "QThread*");
        QVERIFY(idx.isValid());

        QCOMPARE(resetSpy.size(), 0);
    }
};

QTEST_MAIN(MetaTypeModelTest)

#include "metatypemodeltest.moc"
