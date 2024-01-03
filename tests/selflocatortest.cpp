/*
  selflocatortest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
