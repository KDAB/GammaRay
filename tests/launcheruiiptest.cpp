/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Christoph Sterz <christoph.sterz@kdab.com>

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

#include <launcher/ui/connectpage.h>

#include <QtTest/qtest.h>

#include <QDir>
#include <QLineEdit>
#include <QLocalServer>
#include <QSignalSpy>


using namespace GammaRay;

class LauncherUiIPTest : public QObject
{
    Q_OBJECT
private slots:

    void initTestCase()
    {
       //create socket
#ifdef Q_OS_UNIX
      m_localServer.listen("/tmp/socketfile");
#endif
    }

    void cleanupTestCase()
    {
        m_localServer.close();
    }

    void testUrl_data()
    {
        QTest::addColumn<QString>("userInput", nullptr);
        QTest::addColumn<QString>("expectedParsed", nullptr);
        QTest::addColumn<bool>("isValid", nullptr);
        QTest::addColumn<bool>("autoPortWarning", nullptr);

#ifdef Q_OS_UNIX
        QTest::newRow("/tmp/socketfile") << "/tmp/socketfile" << "local:/tmp/socketfile" << true << false;
        QTest::newRow("local:///tmp/socketfile") << "local:///tmp/socketfile" << "local:/tmp/socketfile" << true << false;
#endif

        QTest::newRow("192.168.42.1") << "192.168.42.1" << "tcp://192.168.42.1:11732" << true << true;
        QTest::newRow("tcp://192.168.42.1") << "tcp://192.168.42.1" << "tcp://192.168.42.1:11732" << true << true;
        QTest::newRow("192.168.42") << "192.168.42" << "" << false << true;
        QTest::newRow("192.168.42.1:2342") << "192.168.42.1:2342" << "tcp://192.168.42.1:2342" << true << false;
        QTest::newRow("192.168.42.1:66666") << "192.168.42.1:66666" << "" << false << false;

        QTest::newRow("::1") << "::1" << "tcp://[::1]:11732" << true << true;
        QTest::newRow("fe80::9c0e:f1f4:d51d:a557") << "fe80::9c0e:f1f4:d51d:a557" << "tcp://[fe80::9c0e:f1f4:d51d:a557]:11732" << true << true;
        QTest::newRow("fe80::9c0e:f1f4:d51d:a557%enp0s31f6") << "fe80::9c0e:f1f4:d51d:a557%enp0s31f6" << "tcp://[fe80::9c0e:f1f4:d51d:a557]:11732" << true << true;
        QTest::newRow("fe80::9c0e:f1f4:d51d:a557%enp0s31f6:2342") << "fe80::9c0e:f1f4:d51d:a557%enp0s31f6:2342" << "tcp://[fe80::9c0e:f1f4:d51d:a557]:2342" << true << false;
        QTest::newRow("[fe80::9c0e:f1f4:d51d:a557]") << "[fe80::9c0e:f1f4:d51d:a557]" << "tcp://[fe80::9c0e:f1f4:d51d:a557]:11732" << true << true;
        QTest::newRow("tcp://[fe80::9c0e:f1f4:d51d:a557]:2342") << "tcp://[fe80::9c0e:f1f4:d51d:a557]:2342" << "tcp://[fe80::9c0e:f1f4:d51d:a557]:2342" << true << false;
        QTest::newRow("::ffff:192.168.15.2") << "::ffff:192.168.15.2" << "tcp://[::ffff:192.168.15.2]:11732" << true << true;
    }

    void testUrl()
    {
        QFETCH(QString, userInput);
        QFETCH(QString, expectedParsed);
        QFETCH(bool, isValid);
        QFETCH(bool, autoPortWarning);

        ConnectPage connectPage;
        QSignalSpy addressParsedSpy(&connectPage, SIGNAL(userInputParsed()));
        auto lineEdit = connectPage.findChild<QLineEdit*>("host");
        QVERIFY(lineEdit);
        lineEdit->setText(userInput);
        for(size_t i = 0; i<500; i++) {
            if (addressParsedSpy.count() > 0) break;
            QTest::qWait(1);
        }
        QCOMPARE(connectPage.isValid(), isValid);
        if (!isValid)
            return;
        QCOMPARE(connectPage.m_currentUrl.toString(), expectedParsed);
        QCOMPARE(lineEdit->actions().contains(connectPage.m_implicitPortWarningSign), autoPortWarning);
    }

    void testHostNames_data()
    {
        QTest::addColumn<QString>("userInput", nullptr);
        QTest::addColumn<bool>("isValid", nullptr);

        QTest::newRow("localhost") << "localhost" << true;
        QTest::newRow("tcp://localhost") << "tcp://localhost" << true;
        QTest::newRow("tcp://localhost:11732") << "tcp://localhost:11732" << true;
    }

    void testHostNames()
    {
        QFETCH(QString, userInput);
        QFETCH(bool, isValid);
        ConnectPage connectPage;
        QSignalSpy dnsDoneSpy(&connectPage, SIGNAL(dnsResolved()));
        auto lineEdit = connectPage.findChild<QLineEdit*>("host");
        QVERIFY(lineEdit);
        lineEdit->setText(userInput);
        for(size_t i = 0; i<1000; i++) {
            if (dnsDoneSpy.count() > 0) break;
            QTest::qWait(1);
            if(i == 999) return;
        }
        QCOMPARE(connectPage.isValid(), isValid);
    }


private:
    QLocalServer m_localServer;
};

QTEST_MAIN(LauncherUiIPTest)

#include "launcheruiiptest.moc"
