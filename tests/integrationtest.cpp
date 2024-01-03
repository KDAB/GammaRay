/*
  integrationtest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Kevin Funk <kevin.funk@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
