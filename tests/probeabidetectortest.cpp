/*
  probeabidetectortest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>

#include <launcher/core/probeabi.h>
#include <launcher/core/probeabidetector.h>

#include <QtTest/qtest.h>
#include <QObject>

using namespace GammaRay;

class ProbeABIDetectorTest : public QObject
{
    Q_OBJECT
private slots:
    void testDetectExecutable()
    {
        ProbeABIDetector detector;
        QVERIFY(!detector.qtCoreForExecutable(QCoreApplication::applicationFilePath()).isEmpty());
        const ProbeABI abi = detector.abiForExecutable(QCoreApplication::applicationFilePath());
        QCOMPARE(abi.id(), QStringLiteral(GAMMARAY_PROBE_ABI));
    }

    void testDetectProcess()
    {
        ProbeABIDetector detector;
        QVERIFY(!detector.qtCoreForProcess(QCoreApplication::applicationPid()).isEmpty());
        const ProbeABI abi = detector.abiForProcess(QCoreApplication::applicationPid());
        QCOMPARE(abi.id(), QStringLiteral(GAMMARAY_PROBE_ABI));
    }

    void testContainsQtCore_data()
    {
        QTest::addColumn<QString>("line", nullptr);
        QTest::addColumn<bool>("isQtCore", nullptr);

        QTest::newRow("empty") << QString() << false;
        QTest::newRow("too short1") << "Qt" << false;
        QTest::newRow("too short2") << "Qt5" << false;

        QTest::newRow("unix1") << "libQtCore.so.4.8.6" << true;
        QTest::newRow("unix2") << "libQt5Core.so" << true;
        QTest::newRow("unix3") << "/path/to/libQt6Core.so.6.5.4" << true;
        QTest::newRow("unix4") << "\t   libQt5Core.so.5.4.1\n" << true;

        QTest::newRow("mac1") << "QtCore" << true;
        QTest::newRow("mac2") << "/framework/5/QtCore" << true;
        QTest::newRow("mac3") << "Qt5Core.dylib.5.3.2" << true;
        QTest::newRow("mac4") << "libQt5Core.dylib" << true;
        QTest::newRow("mac5") << "/path/to/QtCore.dylib" << true;
        QTest::newRow("mac debug1") << "QtCore_debug" << true;
        QTest::newRow("mac debug2") << "QtCore_debug.dylib" << true;
        QTest::newRow("mac qt55 framework") << "QtCore.framework/Versions/5/QtCore" << true;

        QTest::newRow("win1") << "QtCore.dll" << true;
        QTest::newRow("win2") << "Qt5Core.dll" << true;
        QTest::newRow("win3") << R"(c:\path\to\Qt6Core.dll)" << true;
        QTest::newRow("win debug1") << "QtCored.dll" << true;
        QTest::newRow("win debug2") << "Qt5Cored.dll" << true;

        QTest::newRow("complex path") << "/Qt/Core/5/QtCore.dll" << true;

        QTest::newRow("addon1") << "QtCoreAddon.dll" << false;
        QTest::newRow("addon2") << "Qt5CoredAddon.so" << false;
        // QTest::newRow("addon3") << "QtCore_Addon.dll" << false;

        QTest::newRow("QT") << "QTCore" << false;
        QTest::newRow("prefix") << "libFooQtCore.so" << false;
        QTest::newRow("libQt") << "libQt.dylib" << false;
    }

    void testContainsQtCore()
    {
        QFETCH(QString, line);
        QFETCH(bool, isQtCore);
        QCOMPARE(ProbeABIDetector::containsQtCore(line.toUtf8()), isQtCore);
    }
};

QTEST_MAIN(ProbeABIDetectorTest)

#include "probeabidetectortest.moc"
