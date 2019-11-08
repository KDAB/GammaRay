/*
  probesettingstest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <launcher/core/launcher.h>
#include <launcher/core/launchoptions.h>
#include <common/paths.h>

#include <QtTest/qtest.h>
#include <QObject>
#include <QSignalSpy>

using namespace GammaRay;

class ProbeSettingsTest : public QObject
{
    Q_OBJECT
private slots:
    void testSettingsRoundtrip()
    {
        Paths::setRootPath(QCoreApplication::applicationDirPath() + QStringLiteral("/.."));

        LaunchOptions options;
        options.setUiMode(LaunchOptions::NoUi);
        options.setProbeSetting("TestValue", "https://www.kdab.com/");
        options.setLaunchArguments(QStringList(QCoreApplication::applicationDirPath() +
                                               QStringLiteral("/probesettingsclient")));

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
