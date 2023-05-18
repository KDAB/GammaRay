/*
  clientconnectiontest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
#include <QSignalSpy>
#include <QTest>

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
        m_process.start(QLatin1String(TESTBIN_DIR "/minimalwidgetapplication"), QStringList {});
        QVERIFY(m_process.waitForStarted());

        QTest::qWait(1000); // give the target some time to actually load the QtCore DLL, otherwise ABI detection fails

        {
            LaunchOptions options;
            options.setUiMode(LaunchOptions::NoUi);
            options.setProbeSetting(QStringLiteral("ServerAddress"), GAMMARAY_DEFAULT_LOCAL_TCP_URL);
            options.setPid(m_process.processId());

            ProbeABIDetector detector;
            options.setProbeABI(ProbeFinder::findBestMatchingABI(detector.abiForProcess(options.pid())));
            Launcher launcher(options);

            QSignalSpy spy(&launcher, &Launcher::attached);
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
        QSignalSpy spyReady(&connector, &ClientConnectionManager::ready);
        QSignalSpy spyError(&connector, &ClientConnectionManager::persistentConnectionError);
        QSignalSpy spyDisconnected(&connector, &ClientConnectionManager::disconnected);

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
            QCOMPARE(m_process.processId(), Endpoint::instance()->pid());
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
