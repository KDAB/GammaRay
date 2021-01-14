/*
  codecmodeltest.cpp

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

#include <plugins/codecbrowser/codecmodel.h>

#include <3rdparty/qt/modeltest.h>

#include <QTest>

using namespace GammaRay;

class CodecModelTest : public QObject
{
    Q_OBJECT
private slots:
    void modelTest()
    {
        AllCodecsModel allCodecs;
        ModelTest tester(&allCodecs);
        QVERIFY(allCodecs.rowCount() > 0);

        SelectedCodecsModel codecs;
        ModelTest tester2(&codecs);
        QCOMPARE(codecs.rowCount(), 0);

        codecs.setCodecs(QStringList() << "UTF-8" << "Latin1");
        QCOMPARE(codecs.rowCount(), 2);
        codecs.updateText("Hello World");

        auto idx = codecs.index(0, 0);
        QCOMPARE(idx.data().toString().toUpper(), QLatin1String("UTF-8"));
        idx = idx.sibling(idx.row(), 1);
        QVERIFY(!idx.data().toString().isEmpty());
    }
};

QTEST_MAIN(CodecModelTest)

#include "codecmodeltest.moc"
