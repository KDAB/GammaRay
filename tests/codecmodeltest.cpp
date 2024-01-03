/*
  codecmodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <plugins/codecbrowser/codecmodel.h>

#include <3rdparty/qt/modeltest.h>

#include <QTest>

using namespace GammaRay;

class CodecModelTest : public QObject
{
    Q_OBJECT
private slots:
    static void modelTest()
    {
        AllCodecsModel allCodecs;
        ModelTest tester(&allCodecs);
        QVERIFY(allCodecs.rowCount() > 0);

        SelectedCodecsModel codecs;
        ModelTest tester2(&codecs);
        QCOMPARE(codecs.rowCount(), 0);

        codecs.setCodecs(QStringList() << "UTF-8"
                                       << "Latin1");
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
