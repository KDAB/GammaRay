/*
  earlyexittest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <config-gammaray.h>

#include <launcher/core/injector/injectorfactory.h>
#include <launcher/core/launchoptions.h>
#include <launcher/core/launcher.h>
#include <launcher/core/probefinder.h>
#include <launcher/core/probeabi.h>

#include <QDebug>
#include <QTest>
#include <QObject>
#include <QSignalSpy>

#include <memory>

using namespace GammaRay;

class EarlyExitTest : public QObject
{
    Q_OBJECT
private:
    static bool hasInjector(const char *type)
    {
        auto injector = InjectorFactory::createInjector(type);
        if (!injector)
            return false;

        return injector->selfTest();
    }

private slots:
    static void testNonExistingTarget()
    {
        LaunchOptions options;
#ifdef Q_OS_MAC
        // older OSX versions fall back to lldb by default, which has no synchronous error reporting, see below test
        options.setInjectorType("preload");
#endif
        options.setUiMode(LaunchOptions::NoUi);
        options.setProbeABI(ProbeFinder::listProbeABIs().at(0));
        options.setWorkingDirectory(QCoreApplication::applicationDirPath());
        options.setLaunchArguments(QStringList() << QStringLiteral("I_DONT_EXIST"));
        options.setProbeSetting(QStringLiteral("ServerAddress"), GAMMARAY_DEFAULT_LOCAL_TCP_URL);
        Launcher launcher(options);
        QVERIFY(!launcher.start());
    }

    void testNonExistingTargetDebugger_data()
    {
        QTest::addColumn<QString>("injectorType", nullptr);
        QTest::newRow("dummy") << QString(); // QTestlib fails when the test data is empty...
        if (hasInjector("gdb"))
            QTest::newRow("gdb") << QStringLiteral("gdb");
        if (hasInjector("lldb"))
            QTest::newRow("lldb") << QStringLiteral("lldb");
    }

    static void testNonExistingTargetDebugger()
    {
        QFETCH(QString, injectorType);
        if (injectorType.isEmpty())
            return;

        LaunchOptions options;
        options.setUiMode(LaunchOptions::NoUi);
        // setting the probe is not strictly needed but we silence a runtime warning this way
        options.setProbeABI(ProbeFinder::listProbeABIs().at(0));
        options.setLaunchArguments(QStringList() << QStringLiteral("I_DONT_EXIST"));
        options.setInjectorType(injectorType);
        Launcher launcher(options);

        QSignalSpy spy(&launcher, SIGNAL(finished()));
        QVERIFY(launcher.start());

        spy.wait(10000);
        QCOMPARE(spy.count(), 1);
        QEXPECT_FAIL("", "Debug injectors miss error detection for this case.", Continue);
        QVERIFY(!launcher.errorMessage().isEmpty());
    }

    static void test()
    {
        LaunchOptions options;
        options.setUiMode(LaunchOptions::NoUi);
        // setting the probe is not strictly needed but we silence a runtime warning this way
        options.setProbeABI(ProbeFinder::listProbeABIs().at(0));
        options.setWorkingDirectory(QCoreApplication::applicationDirPath());
        options.setLaunchArguments(QStringList() << QCoreApplication::applicationDirPath() + QStringLiteral("/sleep") << QStringLiteral("1"));
        options.setProbeSetting(QStringLiteral("ServerAddress"), GAMMARAY_DEFAULT_LOCAL_TCP_URL);
        Launcher launcher(options);

        QSignalSpy spy(&launcher, SIGNAL(finished()));

        QVERIFY(launcher.start());

        spy.wait(10000);

        QCOMPARE(spy.count(), 1);
    }

    void testStop_data()
    {
        QTest::addColumn<QString>("injectorType", nullptr);
        QTest::newRow("default") << QString();
        if (hasInjector("gdb"))
            QTest::newRow("gdb") << QStringLiteral("gdb");
        if (hasInjector("lldb"))
            QTest::newRow("lldb") << QStringLiteral("lldb");
    }

    static void testStop()
    {
        QFETCH(QString, injectorType);

        LaunchOptions options;
        options.setUiMode(LaunchOptions::NoUi);
        // setting the probe is not strictly needed but we silence a runtime warning this way
        options.setProbeABI(ProbeFinder::listProbeABIs().at(0));
        options.setWorkingDirectory(QCoreApplication::applicationDirPath());
        options.setLaunchArguments(QStringList() << QCoreApplication::applicationDirPath() + QStringLiteral("/sleep") << QStringLiteral("1000"));
        options.setInjectorType(injectorType);
        options.setProbeSetting(QStringLiteral("ServerAddress"), GAMMARAY_DEFAULT_LOCAL_TCP_URL);
        Launcher launcher(options);

        QSignalSpy spy(&launcher, SIGNAL(finished()));

        QVERIFY(launcher.start());
        launcher.stop();
        spy.wait(1000);
        QCOMPARE(spy.count(), 1);
    }
};

QTEST_MAIN(EarlyExitTest)

#include "earlyexittest.moc"
