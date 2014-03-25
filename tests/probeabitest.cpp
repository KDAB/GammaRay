/*
  probeabitest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include <common/probeabi.h>

#include <QtTest/qtest.h>
#include <QObject>

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

    abi.setArchitecture("x86_64");
#ifndef Q_OS_WIN
    QVERIFY(abi.isValid());
#else
    QVERIFY(!abi.isValid());
#endif

    abi.setCompiler("MSVC");
    QVERIFY(abi.isValid());
  }

  void testToString_data()
  {
    QTest::addColumn<QString>("id");
    QTest::addColumn<int>("majorVersion");
    QTest::addColumn<int>("minorVersion");
    QTest::addColumn<bool>("isDebug");
    QTest::addColumn<QString>("arch");
    QTest::addColumn<QString>("compiler");

    QTest::newRow("invalid") << QString() << -1 << -1 << false << QString() << QString();
#ifndef Q_OS_WIN
    QTest::newRow("unix") << "qt5.2-x86_64" << 5 << 2 << true << "x86_64" << "GCC";
#else
    QTest::newRow("msvc") << "qt5.2-MSVC-debug-x86_64" << 5 << 2 << true << "x86_64" << "MSVC";
    QTest::newRow("mingw") << "qt5.2-GNU-i686" << 5 << 2 << false << "i686" << "GNU";
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

    ProbeABI abi;
    abi.setQtVersion(majorVersion, minorVersion);
    abi.setIsDebug(isDebug);
    abi.setArchitecture(arch);
    abi.setCompiler(compiler);

    QCOMPARE(abi.id(), id);
  }

  void testFromString_data()
  {
    QTest::addColumn<QString>("id");
    QTest::addColumn<bool>("valid");
    QTest::addColumn<int>("majorVersion");
    QTest::addColumn<int>("minorVersion");
    QTest::addColumn<bool>("isDebug");
    QTest::addColumn<QString>("arch");
    QTest::addColumn<QString>("compiler");

    QTest::newRow("invalid") << QString() << false << -1 << -1 << false << QString() << QString();
    QTest::newRow("only version") << "qt5.2" << false << -1 << -1 << false << QString() << QString();
    QTest::newRow("too many items") << "qt5.2-some-random-stuff-with-too-many-dashs" << false << -1 << -1 << false << QString() << QString();
    QTest::newRow("missing debug/release") << "qt5.2-MSVC-i686" << false << -1 << -1 << false << QString() << QString();
    QTest::newRow("extra debug/release") << "qt5.2-GNU-debug-arm" << false << -1 << -1 << false << QString() << QString();
#ifndef Q_OS_WIN
    QTest::newRow("unix") << "qt5.2-x86_64" << true << 5 << 2 << true << "x86_64" << "GCC";
#else
    QTest::newRow("msvc") << "qt5.2-MSVC-debug-x86_64" << true << 5 << 2 << true << "x86_64" << "MSVC";
    QTest::newRow("mingw") << "qt5.2-GNU-i686" << true << 5 << 2 << true << "i686" << "GNU";
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

    const ProbeABI abi = ProbeABI::fromString(id);
    QCOMPARE(abi.isValid(), valid);
    if (!valid)
      return;

    QCOMPARE(abi.majorQtVersion(), majorVersion);
    QCOMPARE(abi.minorQtVersion(), minorVersion);
    QCOMPARE(abi.architecture(), arch);
    if (abi.isDebugRelevant())
      QCOMPARE(abi.isDebug(), isDebug);
#ifdef Q_OS_WIN
    QCOMPARE(abi.compiler(), compiler);
#else
    Q_UNUSED(compiler);
#endif
  }

  void testDisplayString_data()
  {
    QTest::addColumn<QString>("id");
    QTest::addColumn<QString>("display");

    QTest::newRow("invalid") << QString() << QString();
#ifndef Q_OS_WIN
    QTest::newRow("unix") << "qt5.2-x86_64" << "Qt 5.2 (x86_64)";
#else
    QTest::newRow("msvc") << "qt5.2-MSVC-debug-x86_64" << "Qt 5.2 (MSVC, debug, x86_64)";
    QTest::newRow("mingw") << "qt5.2-GNU-i686" << "Qt 5.2 (GNU, i686)";
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
    const ProbeABI targetABI = ProbeABI::fromString("qt5.2-x86_64");
    const ProbeABI probeABI = ProbeABI::fromString("qt5.1-x86_64");
    const bool compilerAbiMatters = false;
#else
    const ProbeABI targetABI = ProbeABI::fromString("qt5.2-MSVC-release-x86_64");
    const ProbeABI probeABI = ProbeABI::fromString("qt5.1-MSVC-release-x86_64");
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
    incompatABI.setArchitecture("i686");
    QVERIFY(!targetABI.isCompatible(incompatABI));

    // different debug/release mode
    incompatABI = targetABI;
    incompatABI.setIsDebug(true);
    QCOMPARE(targetABI.isCompatible(incompatABI), !compilerAbiMatters);

    // different compiler
    incompatABI = targetABI;
    incompatABI.setCompiler("Clang");
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
