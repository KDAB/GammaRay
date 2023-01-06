/*
  integrationtest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "baseprobetest.h"

#include <QCoreApplication>
#include <QMetaObject>
#include <QObject>
#include <QTest>

using namespace GammaRay;

namespace {
int argc = 1;
char argv0[] = "integrationtest";
char *argv[] = { argv0 };
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
