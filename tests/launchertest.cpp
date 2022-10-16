/*
  launchertest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <config-gammaray.h>
#include <gammaray-test-config.h>

#include "testhelpers.h"

#include <launcher/core/injector/injectorfactory.h>
#include <launcher/core/launchoptions.h>
#include <launcher/core/launcher.h>
#include <launcher/core/probefinder.h>
#include <launcher/core/probeabi.h>
#include <launcher/core/probeabidetector.h>

#include <QDebug>
#include <QObject>
#include <QSignalSpy>
#include <QTest>

#include <memory>

#ifdef Q_OS_WIN32
#include <qt_windows.h>
#endif

using namespace GammaRay;
using namespace TestHelpers;

class LauncherTest : public QObject
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
    void testLauncher_data()
    {
        QTest::addColumn<QString>("injectorType", nullptr);
        QTest::newRow("dummy") << QString(); // workaround for QTestlib asserting on empty test data sets
        if (hasInjector("preload"))
            QTest::newRow("preload") << QStringLiteral("preload");
        if (hasInjector("windll"))
            QTest::newRow("windll") << QStringLiteral("windll");
    }

    static void testLauncher()
    {
        QFETCH(QString, injectorType);
        if (injectorType.isEmpty())
            return;

        LaunchOptions options;
        options.setUiMode(LaunchOptions::NoUi);
        QString exePath = QLatin1String(TESTBIN_DIR "/minimalcoreapplication");
#ifdef Q_OS_WIN
        exePath += QLatin1String(".exe");
#endif
        ProbeABIDetector detector;
        options.setProbeABI(ProbeFinder::findBestMatchingABI(detector.abiForExecutable(exePath)));
        options.setLaunchArguments(QStringList() << exePath);
        options.setInjectorType(injectorType);
        options.setProbeSetting(QStringLiteral("ServerAddress"), GAMMARAY_DEFAULT_LOCAL_TCP_URL);
        Launcher launcher(options);

        QSignalSpy startSpy(&launcher, SIGNAL(started()));
        QVERIFY(startSpy.isValid());
        QSignalSpy finishSpy(&launcher, SIGNAL(finished()));
        QVERIFY(finishSpy.isValid());

        QVERIFY(launcher.start());
        startSpy.wait(60000);
        QCOMPARE(startSpy.count(), 1);
        QCOMPARE(finishSpy.count(), 0);

        launcher.stop();
        finishSpy.wait(10000);
        QCOMPARE(finishSpy.count(), 1);
    }

#ifdef HAVE_QT_WIDGETS
    static void testLauncherStyle()
    {
        LaunchOptions options;
        options.setUiMode(LaunchOptions::NoUi);
        QString exePath = QLatin1String(TESTBIN_DIR "/minimalwidgetapplication");
#ifdef Q_OS_WIN
        exePath += QLatin1String(".exe");
#endif
        ProbeABIDetector detector;
        options.setProbeABI(ProbeFinder::findBestMatchingABI(detector.abiForExecutable(exePath)));
        options.setLaunchArguments(QStringList() << exePath);
        options.setInjectorType(QStringLiteral("style"));
        options.setProbeSetting(QStringLiteral("ServerAddress"), GAMMARAY_DEFAULT_LOCAL_TCP_URL);
        Launcher launcher(options);

        QSignalSpy startSpy(&launcher, SIGNAL(started()));
        QVERIFY(startSpy.isValid());
        QSignalSpy finishSpy(&launcher, SIGNAL(finished()));
        QVERIFY(finishSpy.isValid());

        QVERIFY(launcher.start());
        startSpy.wait(60000);
        QCOMPARE(startSpy.count(), 1);
        QCOMPARE(finishSpy.count(), 0);

        launcher.stop();
        finishSpy.wait(10000);
        QCOMPARE(finishSpy.count(), 1);
    }
#endif

    static void testAttach()
    {
        QProcess target;
        target.setProcessChannelMode(QProcess::ForwardedChannels);
        target.start(QLatin1String(TESTBIN_DIR "/minimalcoreapplication"));
        QVERIFY(target.waitForStarted());

        LaunchOptions options;
        options.setUiMode(LaunchOptions::NoUi);
        options.setProbeSetting(QStringLiteral("ServerAddress"), GAMMARAY_DEFAULT_LOCAL_TCP_URL);
        options.setPid(target.processId());
        QTest::qWait(5000); // give the target some time to actually load the QtCore DLL, otherwise ABI detection fails
        ProbeABIDetector detector;
        options.setProbeABI(ProbeFinder::findBestMatchingABI(detector.abiForProcess(options.pid())));
        Launcher launcher(options);

        QSignalSpy spy(&launcher, SIGNAL(attached()));
        QVERIFY(spy.isValid());
        QVERIFY(launcher.start());

        spy.wait(30000);
        QCOMPARE(spy.count(), 1);

        target.kill();
        target.waitForFinished();
    }
};

QTEST_MAIN(LauncherTest)

#include "launchertest.moc"
