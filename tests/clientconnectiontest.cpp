/*
  clientconnectiontest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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
#include <gammaray-test-config.h>

#include "testhelpers.h"

#include <client/clientconnectionmanager.h>

#include <common/endpoint.h>

#include <launcher/core/launcher.h>
#include <launcher/core/launchoptions.h>
#include <launcher/core/probeabidetector.h>
#include <launcher/core/probefinder.h>

#include <QProcess>
#include <QtTest>

#ifdef Q_OS_WIN
#include <qt_windows.h>
#endif

using namespace GammaRay;
using namespace TestHelpers;

class ClientConnectionTest : public QObject
{
    Q_OBJECT

public:
    explicit ClientConnectionTest(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private slots:
    void initTestCase()
    {
        ClientConnectionManager::init();

        m_process.setProcessChannelMode(QProcess::ForwardedChannels);
        m_process.start(QLatin1String(TESTBIN_DIR "/minimalwidgetapplication"));
        QVERIFY(m_process.waitForStarted());

        QTest::qWait(1000); // give the target some time to actually load the QtCore DLL, otherwise ABI detection fails

        {
            LaunchOptions options;
            options.setUiMode(LaunchOptions::NoUi);
            options.setProbeSetting(QStringLiteral("ServerAddress"), GAMMARAY_DEFAULT_LOCAL_TCP_URL);
#ifdef Q_OS_WIN
            options.setPid(m_process.pid()->dwProcessId);
#else
            options.setPid(m_process.pid());
#endif

            ProbeABIDetector detector;
            options.setProbeABI(ProbeFinder::findBestMatchingABI(detector.abiForProcess(options.pid())));
            Launcher launcher(options);

            QSignalSpy spy(&launcher, SIGNAL(attached()));
            QVERIFY(spy.isValid());
            QVERIFY(launcher.start());

            spy.wait(30000);
            QCOMPARE(spy.count(), 1);

            m_serverUrl = launcher.serverAddress();
            QVERIFY(m_serverUrl.isValid());
            QVERIFY(m_serverUrl.port() != -1);
        }
    }

    void testMultipleConnection()
    {
        ClientConnectionManager connector(nullptr, false);
        QSignalSpy spyReady(&connector, SIGNAL(ready()));
        QSignalSpy spyError(&connector, SIGNAL(persistentConnectionError(QString)));
        QSignalSpy spyDisconnected(&connector, SIGNAL(disconnected()));

        QVERIFY(spyReady.isValid());
        QVERIFY(spyError.isValid());
        QVERIFY(spyDisconnected.isValid());

        for (int i = 0; i < 5; ++i) {
            qWarning("Phase: #%i", i);

            spyReady.clear();
            spyError.clear();
            spyDisconnected.clear();

            connector.connectToHost(m_serverUrl);

            int loops = 0;
            while (loops++ < (30000 / 10)) {
                QTest::qWait(10);

                if (!spyReady.isEmpty()
                        || !spyError.isEmpty()
                        || !spyDisconnected.isEmpty()) {
                    break;
                }
            }

            QVERIFY(spyReady.count() == 1);
            QVERIFY(Endpoint::isConnected());
#ifdef Q_OS_WIN
            QVERIFY(m_process.pid()->dwProcessId == Endpoint::instance()->pid());
#else
            QVERIFY(m_process.pid() == Endpoint::instance()->pid());
#endif
            QVERIFY(spyError.isEmpty());
            QVERIFY(spyDisconnected.isEmpty());

            connector.disconnectFromHost();
            QTest::qWait(1);

            QVERIFY(spyDisconnected.count() == 1);
            QVERIFY(!Endpoint::isConnected());
        }
    }

    void cleanupTestCase()
    {
        QVERIFY(m_process.state() == QProcess::Running);
        m_process.terminate();
        m_process.kill();
        QVERIFY(m_process.waitForFinished(5000));
    }

private:
    QProcess m_process;
    QUrl m_serverUrl;
};

QTEST_MAIN(ClientConnectionTest)

#include "clientconnectiontest.moc"
