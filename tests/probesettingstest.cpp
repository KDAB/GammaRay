/*
  probesettingstest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <config-gammaray.h>

#include <launcher/core/launcher.h>
#include <launcher/core/launchoptions.h>
#include <common/paths.h>

#include <QObject>
#include <QSignalSpy>
#include <QTest>

using namespace GammaRay;

class ProbeSettingsTest : public QObject
{
    Q_OBJECT
private slots:
    static void testSettingsRoundtrip()
    {
        Paths::setRootPath(QCoreApplication::applicationDirPath() + QStringLiteral("/.."));

        LaunchOptions options;
        options.setUiMode(LaunchOptions::NoUi);
        options.setProbeSetting("TestValue", "https://www.kdab.com/");
        options.setLaunchArguments(QStringList(QCoreApplication::applicationDirPath() + QStringLiteral("/probesettingsclient")));

        // this will effectively disable injection, so we will just launch the process
        options.setProbePath(QCoreApplication::applicationDirPath());
        options.setInjectorType(QStringLiteral("style"));
        options.setProbeSetting(QStringLiteral("ServerAddress"), GAMMARAY_DEFAULT_LOCAL_TCP_URL);

        Launcher launcher(options);
        QSignalSpy startSpy(&launcher, SIGNAL(started()));
        QVERIFY(startSpy.isValid());
        QSignalSpy finishSpy(&launcher, SIGNAL(finished()));
        QVERIFY(finishSpy.isValid());

        QVERIFY(launcher.start());
        QVERIFY(finishSpy.wait());
        QCOMPARE(startSpy.size(), 1);
        QCOMPARE(finishSpy.size(), 1);

        QCOMPARE(launcher.serverAddress(), QUrl(QStringLiteral("https://www.kdab.com/")));
    }
};

QTEST_MAIN(ProbeSettingsTest)

#include "probesettingstest.moc"
