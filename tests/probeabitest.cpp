/*
  probeabitest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <launcher/core/probeabi.h>

#include <QObject>
#include <QTest>

using namespace GammaRay;

class ProbeABITest : public QObject
{
    Q_OBJECT
private slots:
    void testIsValid()
    {
        ProbeABI abi;
        QVERIFY(!abi.isValid());

        abi.setQtVersion(5, 2);
        QVERIFY(!abi.isValid());

        abi.setArchitecture(QStringLiteral("x86_64"));
#ifndef Q_OS_WIN
        QVERIFY(abi.isValid());
#else
        QVERIFY(!abi.isValid());
#endif

        abi.setCompiler(QStringLiteral("MSVC"));
        abi.setCompilerVersion(QStringLiteral("140"));
        QVERIFY(abi.isValid());
    }

    void testToString_data()
    {
        QTest::addColumn<QString>("id", nullptr);
        QTest::addColumn<int>("majorVersion", nullptr);
        QTest::addColumn<int>("minorVersion", nullptr);
        QTest::addColumn<bool>("isDebug", nullptr);
        QTest::addColumn<QString>("arch", nullptr);
        QTest::addColumn<QString>("compiler", nullptr);
        QTest::addColumn<QString>("compilerVersion", nullptr);

        QTest::newRow("invalid") << QString() << -1 << -1 << false << QString() << QString() << QString();
#ifndef Q_OS_WIN
#if defined(Q_OS_MAC)
        QTest::newRow("mac") << "qt5_2-x86_64_debug" << 5 << 2 << true << "x86_64" << "CLANG" << QString();
#else
        QTest::newRow("unix") << "qt5_2-x86_64" << 5 << 2 << true << "x86_64" << "GCC" << QString();
#endif
#else
        QTest::newRow("msvc") << "qt5_2-MSVC-140-x86_64d" << 5 << 2 << true << "x86_64" << "MSVC" << "140";
        QTest::newRow("mingw") << "qt5_2-GNU-i686" << 5 << 2 << false << "i686" << "GNU" << QString();
#endif
    }

    void testToString()
    {
        QFETCH(QString, id);
        QFETCH(int, majorVersion);
        QFETCH(int, minorVersion);
        QFETCH(bool, isDebug);
        QFETCH(QString, arch);
        QFETCH(QString, compiler);
        QFETCH(QString, compilerVersion);

        ProbeABI abi;
        abi.setQtVersion(majorVersion, minorVersion);
        abi.setIsDebug(isDebug);
        abi.setArchitecture(arch);
        abi.setCompiler(compiler);
        abi.setCompilerVersion(compilerVersion);

        QCOMPARE(abi.id(), id);
    }

    void testFromString_data()
    {
        QTest::addColumn<QString>("id", nullptr);
        QTest::addColumn<bool>("valid", nullptr);
        QTest::addColumn<int>("majorVersion", nullptr);
        QTest::addColumn<int>("minorVersion", nullptr);
        QTest::addColumn<bool>("isDebug", nullptr);
        QTest::addColumn<QString>("arch", nullptr);
        QTest::addColumn<QString>("compiler", nullptr);
        QTest::addColumn<QString>("compilerVersion", nullptr);

        QTest::newRow("invalid") << QString() << false << -1 << -1 << false << QString()
                                 << QString() << QString();
        QTest::newRow("only version") << "qt5_2" << false << -1 << -1 << false << QString()
                                      << QString() << QString();
        QTest::newRow("too many items") << "qt5_2-some-random-stuff-with-too-many-dashs" << false
                                        << -1 << -1 << false << QString() << QString() << QString();
        QTest::newRow("extra debug/release") << "qt5_2-MSVC-140-release-i686" << false << -1 << -1
                                             << false << QString() << QString() << QString();
        QTest::newRow("extra debug/release") << "qt5_2-GNU-debug-arm" << false << -1 << -1
                                             << false << QString() << QString() << QString();
#ifndef Q_OS_WIN
#if defined(Q_OS_MAC)
        QTest::newRow("mac") << "qt5_2-x86_64_debug" << true << 5 << 2 << true << "x86_64"
                             << "CLANG" << QString();
#else
        QTest::newRow("unix") << "qt5_2-x86_64" << true << 5 << 2 << true << "x86_64" << "GCC" << QString();
#endif
#else
        QTest::newRow("msvc") << "qt5_2-MSVC-140-x86_64d" << true << 5 << 2 << true << "x86_64"
                              << "MSVC" << "140";
        QTest::newRow("mingw") << "qt5_2-GNU-i686d" << true << 5 << 2 << true << "i686" << "GNU" << QString();
#endif
    }

    void testFromString()
    {
        QFETCH(QString, id);
        QFETCH(bool, valid);
        QFETCH(int, majorVersion);
        QFETCH(int, minorVersion);
        QFETCH(bool, isDebug);
        QFETCH(QString, arch);
        QFETCH(QString, compiler);
#ifdef Q_OS_WIN
        QFETCH(QString, compilerVersion);
#endif
        const ProbeABI abi = ProbeABI::fromString(id);
        QCOMPARE(abi.isValid(), valid);
        if (!valid)
            return;

        QCOMPARE(abi.majorQtVersion(), majorVersion);
        QCOMPARE(abi.minorQtVersion(), minorVersion);
        QCOMPARE(abi.architecture(), arch);
        if (abi.isDebugRelevant()) {
            QCOMPARE(abi.isDebug(), isDebug);
        }
#ifdef Q_OS_WIN
        QCOMPARE(abi.compiler(), compiler);
        QCOMPARE(abi.compilerVersion(), compilerVersion);
#else
        Q_UNUSED(compiler);
#endif
    }

    void testDisplayString_data()
    {
        QTest::addColumn<QString>("id", nullptr);
        QTest::addColumn<QString>("display", nullptr);

        QTest::newRow("invalid") << QString() << QString();
#ifndef Q_OS_WIN
#if defined(Q_OS_MAC)
        QTest::newRow("mac") << "qt5_2-x86_64_debug" << "Qt 5.2 (debug, x86_64)";
#else
        QTest::newRow("unix") << "qt5_2-x86_64" << "Qt 5.2 (x86_64)";
#endif
#else
        QTest::newRow("msvc") << "qt5_2-MSVC-140-x86_64d" << "Qt 5.2 (MSVC, 140, debug, x86_64)";
        QTest::newRow("mingw") << "qt5_2-GNU-i686d" << "Qt 5.2 (GNU, debug, i686)";
#endif
    }

    void testDisplayString()
    {
        QFETCH(QString, id);
        QFETCH(QString, display);

        const ProbeABI abi = ProbeABI::fromString(id);
        QCOMPARE(abi.displayString(), display);
    }

    void testProbeABICompat()
    {
#ifndef Q_OS_WIN
        const ProbeABI targetABI = ProbeABI::fromString(QStringLiteral("qt5_2-x86_64"));
        const ProbeABI probeABI = ProbeABI::fromString(QStringLiteral("qt5_1-x86_64"));
#if defined(Q_OS_MAC)
        const bool debugAbiMatters = true;
#else
        const bool debugAbiMatters = false;
#endif
        const bool compilerAbiMatters = false;
#else
        const ProbeABI targetABI = ProbeABI::fromString(QStringLiteral("qt5_2-MSVC-140-x86_64"));
        const ProbeABI probeABI = ProbeABI::fromString(QStringLiteral("qt5_1-MSVC-140-x86_64"));
        const bool debugAbiMatters = true;
        const bool compilerAbiMatters = true;
#endif

        // full match, or same major version and older probe
        QVERIFY(targetABI.isCompatible(targetABI));
        QVERIFY(targetABI.isCompatible(probeABI));

        // incompatible
        // newer minor version probe
        QVERIFY(!probeABI.isCompatible(targetABI));

        // different major version
        ProbeABI incompatABI(probeABI);
        incompatABI.setQtVersion(4, 8);
        QVERIFY(!targetABI.isCompatible(incompatABI));
        QVERIFY(!incompatABI.isCompatible(targetABI));

        // different architecture
        incompatABI = targetABI;
        incompatABI.setArchitecture(QStringLiteral("i686"));
        QVERIFY(!targetABI.isCompatible(incompatABI));

        // different debug/release mode
        incompatABI = targetABI;
        incompatABI.setIsDebug(true);
        QCOMPARE(targetABI.isCompatible(incompatABI), !debugAbiMatters);

        // different compiler
        incompatABI = targetABI;
        incompatABI.setCompiler(QStringLiteral("Clang"));
        QCOMPARE(targetABI.isCompatible(incompatABI), !compilerAbiMatters);
    }

    void testProbeABISort()
    {
        ProbeABI qt52;
        qt52.setQtVersion(5, 2);
        ProbeABI qt51;
        qt51.setQtVersion(5, 1);
        ProbeABI qt48;
        qt48.setQtVersion(4, 8);

        QVERIFY(qt51 < qt52);
        QVERIFY(qt48 < qt52);
        QVERIFY(!(qt52 < qt51));
        QVERIFY(!(qt51 < qt48));

        QVERIFY(!(qt52 < qt52));
    }
};

QTEST_MAIN(ProbeABITest)

#include "probeabitest.moc"
