/*
  executiontest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>

#include <core/execution.h>

#include <QDebug>
#include <QObject>
#include <QTest>

using namespace GammaRay;

class ExecutionTest : public QObject
{
    Q_OBJECT
private slots:
    void testReadOnlyData()
    {
        QVERIFY(Execution::isReadOnlyData(&staticMetaObject));
        QVERIFY(Execution::isReadOnlyData(&QObject::staticMetaObject));
        QVERIFY(!Execution::isReadOnlyData(this));
        QVERIFY(Execution::isReadOnlyData(this->metaObject()));
        QVERIFY(Execution::isReadOnlyData(this->metaObject()->superClass()));
    }

    static void benchmarkReadOnlyData()
    {
        QBENCHMARK
        {
            Execution::isReadOnlyData(&QObject::staticMetaObject);
        }
    }

    static void testStackTrace()
    {
        if (!Execution::stackTracingAvailable())
            return;
        const auto trace = Execution::stackTrace(32);
        QVERIFY(trace.size() > 0);
        const auto resolved = Execution::resolveAll(trace);
        QCOMPARE(resolved.size(), trace.size());
        for (const auto &frame : resolved) {
            qDebug() << frame.name << frame.location.displayString();
        }
    }

    static void benchmarkStackTrace()
    {
        if (!Execution::stackTracingAvailable())
            return;
        QBENCHMARK
        {
            const auto trace = Execution::stackTrace(255);
            QVERIFY(trace.size() > 0);
        }
    }

    static void benchmarkResolveStackTrace()
    {
        if (!Execution::stackTracingAvailable())
            return;
        const auto trace = Execution::stackTrace(255);
        QVERIFY(trace.size() > 0);
        QBENCHMARK
        {
            const auto frames = Execution::resolveAll(trace);
            QCOMPARE(frames.size(), trace.size());
        }
    }
};

QTEST_MAIN(ExecutionTest)

#include "executiontest.moc"
