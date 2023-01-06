/*
  selftesttest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
