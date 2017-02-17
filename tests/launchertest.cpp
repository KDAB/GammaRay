/*
  launchertest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "config-gammaray.h"
#include "gammaray-test-config.h"

#include <launcher/injector/injectorfactory.h>
#include <launcher/launchoptions.h>
#include <launcher/launcher.h>
#include <launcher/probefinder.h>
#include <launcher/probeabi.h>

#include <QDebug>
#include <QtTest/qtest.h>
#include <QObject>
#include <QSignalSpy>

#include <memory>

#ifdef Q_OS_WIN32
#include <qt_windows.h>
#endif

using namespace GammaRay;

class LauncherTest : public QObject
{
    Q_OBJECT
private:
    bool hasInjector(const char *type) const
    {
        auto injector = InjectorFactory::createInjector(type);
        if (!injector)
            return false;

        return injector->selfTest();
    }

    void waitForSpy(QSignalSpy *spy, int timeout)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        spy->wait(timeout);
#else
        int loops = 0;
        while (loops++ < (timeout / 10)) {
            if (spy->count() >= 1)
                break;
            QTest::qWait(10);
        }
#endif
    }

private slots:
    void testLauncher_data()
    {
        QTest::addColumn<QString>("injectorType");
        QTest::newRow("dummy") << QString(); // workaround for QTestlib asserting on empty test data sets
#if !defined(Q_OS_MAC) || QT_VERSION >= QT_VERSION_CHECK(5, 1, 0) // Requires at least Q_COREAPP_STARTUP_FUNCTION to work on macOS
        if (hasInjector("preload"))
            QTest::newRow("preload") << QStringLiteral("preload");
#endif
#if !defined(Q_OS_WIN) || QT_VERSION >= QT_VERSION_CHECK(5, 1, 0) // Requires at least Q_COREAPP_STARTUP_FUNCTION to work on Windows
        if (hasInjector("windll"))
            QTest::newRow("windll") << QStringLiteral("windll");
#endif
    }

    void testLauncher()
    {
        QFETCH(QString, injectorType);
        if (injectorType.isEmpty())
            return;

        LaunchOptions options;
        options.setUiMode(LaunchOptions::NoUi);
        // setting the probe is not strictly needed but we silence a runtime warning this way
        options.setProbeABI(ProbeFinder::listProbeABIs().at(0));
        options.setLaunchArguments(QStringList() << QLatin1String(TESTBIN_DIR "/minimalcoreapplication"));
        options.setInjectorType(injectorType);
        options.setProbeSetting(QStringLiteral("ServerAddress"), GAMMARAY_DEFAULT_LOCAL_TCP_URL);
        Launcher launcher(options);

        QSignalSpy startSpy(&launcher, SIGNAL(started()));
        QVERIFY(startSpy.isValid());
        QSignalSpy finishSpy(&launcher, SIGNAL(finished()));
        QVERIFY(finishSpy.isValid());

        QVERIFY(launcher.start());
        waitForSpy(&startSpy, 1000);
        QCOMPARE(startSpy.count(), 1);
        QCOMPARE(finishSpy.count(), 0);

        launcher.stop();
        waitForSpy(&finishSpy, 1000);
        QCOMPARE(finishSpy.count(), 1);
    }

#ifdef HAVE_QT_WIDGETS
    void testLauncherStyle()
    {
        LaunchOptions options;
        options.setUiMode(LaunchOptions::NoUi);
        // setting the probe is not strictly needed but we silence a runtime warning this way
        options.setProbeABI(ProbeFinder::listProbeABIs().at(0));
        options.setLaunchArguments(QStringList() << QLatin1String(TESTBIN_DIR "/minimalwidgetapplication"));
        options.setInjectorType(QStringLiteral("style"));
        options.setProbeSetting(QStringLiteral("ServerAddress"), GAMMARAY_DEFAULT_LOCAL_TCP_URL);
        Launcher launcher(options);

        QSignalSpy startSpy(&launcher, SIGNAL(started()));
        QVERIFY(startSpy.isValid());
        QSignalSpy finishSpy(&launcher, SIGNAL(finished()));
        QVERIFY(finishSpy.isValid());

        QVERIFY(launcher.start());
        waitForSpy(&startSpy, 10000);
        QCOMPARE(startSpy.count(), 1);
        QCOMPARE(finishSpy.count(), 0);

        launcher.stop();
        waitForSpy(&finishSpy, 10000);
        QCOMPARE(finishSpy.count(), 1);
    }
#endif

    void testAttach()
    {
        QProcess target;
        target.setProcessChannelMode(QProcess::ForwardedChannels);
        target.start(QLatin1String(TESTBIN_DIR "/minimalcoreapplication"));
        QVERIFY(target.waitForStarted());

        LaunchOptions options;
        options.setUiMode(LaunchOptions::NoUi);
        options.setProbeABI(ProbeFinder::listProbeABIs().at(0));
        options.setProbeSetting(QStringLiteral("ServerAddress"), GAMMARAY_DEFAULT_LOCAL_TCP_URL);
#ifdef Q_OS_WIN32
         options.setPid(target.pid()->dwProcessId);
#else
        options.setPid(target.pid());
#endif
        Launcher launcher(options);

        QSignalSpy spy(&launcher, SIGNAL(attached()));
        QVERIFY(spy.isValid());
        QVERIFY(launcher.start());

        waitForSpy(&spy, 30000);
        QCOMPARE(spy.count(), 1);

        target.kill();
        target.waitForFinished();
    }
};

QTEST_MAIN(LauncherTest)

#include "launchertest.moc"
