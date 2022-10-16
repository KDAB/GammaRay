/*
  selflocatortest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <config-gammaray.h>

#include <common/selflocator.cpp>
#include <common/paths.h>

#include <QDebug>
#include <QObject>
#include <QTest>

using namespace GammaRay;

class SelfLocatorTest : public QObject
{
    Q_OBJECT
private slots:
    static void testFindMe()
    {
        qDebug() << SelfLocator::findMe();
        QCOMPARE(QCoreApplication::applicationFilePath(), SelfLocator::findMe());
    }

    static void testRootPath()
    {
        qDebug() << Paths::rootPath();
        QVERIFY(!Paths::rootPath().isEmpty());
    }
};

QTEST_MAIN(SelfLocatorTest)

#include "selflocatortest.moc"
