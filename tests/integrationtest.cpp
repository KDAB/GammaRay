/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "baseprobetest.h"

#include <QCoreApplication>
#include <QMetaObject>
#include <QObject>
#include <QTest>

using namespace GammaRay;

namespace
{
    int argc = 1;
    char argv0[] = "integrationtest";
    char* argv[] = {argv0};
}

class IntegrationTest : public BaseProbeTest
{
    Q_OBJECT

private slots:
    void runQCoreApplication()
    {
        {
            QCoreApplication app(argc, argv);
            createProbe();

            QMetaObject::invokeMethod(&app, "quit", Qt::QueuedConnection);
            app.exec();
        }
        QVERIFY(!Probe::instance());
    }

    void runQCoreApplicationWithoutExec()
    {
        {
            QCoreApplication app(argc, argv);
            createProbe();
        }
        QVERIFY(!Probe::instance());
    }
};

QTEST_APPLESS_MAIN(IntegrationTest)

#include "integrationtest.moc"
