/*
  methodmodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
    MY_TAG void taggedSlot() {}
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    Q_REVISION(147) void revisionedSlot() {}
#else
    Q_REVISION(0, 147) void revisionedSlot() {}
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

    void testModel()
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

    void testToolTip_data()
    {
        QTest::addColumn<QString>("name", nullptr);
        QTest::addColumn<QString>("toolTip", nullptr);

        QTest::newRow("tagged") << "taggedSlot" << "MY_TAG";
        QTest::newRow("revision") << "revisionedSlot" << "147";
    }

    void testToolTip()
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
