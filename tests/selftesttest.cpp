/*
  selftesttest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>

#include <launcher/core/selftest.h>
#include <common/paths.h>

#include <QObject>
#include <QTest>

using namespace GammaRay;

class SelfTestTest : public QObject
{
    Q_OBJECT
private slots:
    static void runSelfTest()
    {
        Paths::setRelativeRootPath(GAMMARAY_INVERSE_BIN_DIR);
        SelfTest selfTest;
        QVERIFY(selfTest.checkEverything());
    }
};

QTEST_MAIN(SelfTestTest)

#include "selftesttest.moc"
