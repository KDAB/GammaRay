/*
  executiontest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
