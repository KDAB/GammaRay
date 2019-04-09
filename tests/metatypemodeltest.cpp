/*
  metatypemodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "testhelpers.h"

#include <config-gammaray.h>

#include <core/tools/metatypebrowser/metatypesmodel.h>
#include <ui/tools/metatypebrowser/metatypesclientmodel.h>
#include <common/tools/metatypebrowser/metatyperoles.h>

#include <common/objectid.h>

#include <3rdparty/qt/modeltest.h>

#include <QDebug>
#include <QObject>
#include <QSignalSpy>
#include <QThread>
#include <QtTest/qtest.h>

using namespace GammaRay;
using namespace TestHelpers;

class MetaTypeModelTest : public QObject
{
    Q_OBJECT
private slots:
    void testModel()
    {
        MetaTypesModel srcModel;
        MetaTypesClientModel model;
        model.setSourceModel(&srcModel);
        ModelTest modelTest(&model);
        QVERIFY(model.rowCount() > 0);
        QSignalSpy resetSpy(&model, SIGNAL(modelReset()));
        QVERIFY(resetSpy.isValid());

        auto idx = searchFixedIndex(&model, "QObject*");
        QVERIFY(idx.isValid());
        QVERIFY(!idx.data(MetaTypeRoles::MetaObjectIdRole).value<ObjectId>().isNull());

        idx = searchFixedIndex(&model, "int");
        QVERIFY(idx.isValid());
        QVERIFY(idx.data(MetaTypeRoles::MetaObjectIdRole).isNull());

        idx = searchFixedIndex(&model, "QThread*");
        QVERIFY(!idx.isValid());
        qRegisterMetaType<QThread*>();
        srcModel.scanMetaTypes();
        idx = searchFixedIndex(&model, "QThread*");
        QVERIFY(idx.isValid());

        QCOMPARE(resetSpy.size(), 0);
    }

};

QTEST_MAIN(MetaTypeModelTest)

#include "metatypemodeltest.moc"
