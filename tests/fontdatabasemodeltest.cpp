/*
  fontdatabasemodeltest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <plugins/fontbrowser/fontdatabasemodel.h>

#include <3rdparty/qt/modeltest.h>

#include <QTest>

using namespace GammaRay;

class FontDatabaseModelTest : public QObject
{
    Q_OBJECT
private slots:
    static void modelTest()
    {
        FontDatabaseModel model;
        ModelTest tester(&model);

        QVERIFY(model.rowCount() > 0);
    }
};

QTEST_MAIN(FontDatabaseModelTest)

#include "fontdatabasemodeltest.moc"
