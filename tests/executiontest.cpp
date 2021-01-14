/*
  executiontest.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

    void benchmarkReadOnlyData()
    {
        QBENCHMARK {
            Execution::isReadOnlyData(&QObject::staticMetaObject);
        }
    }

    void testStackTrace()
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

    void benchmarkStackTrace()
    {
        if (!Execution::stackTracingAvailable())
            return;
        QBENCHMARK {
            const auto trace = Execution::stackTrace(255);
            QVERIFY(trace.size() > 0);
        }
    }

    void benchmarkResolveStackTrace()
    {
        if (!Execution::stackTracingAvailable())
            return;
        const auto trace = Execution::stackTrace(255);
        QVERIFY(trace.size() > 0);
        QBENCHMARK {
            const auto frames = Execution::resolveAll(trace);
            QCOMPARE(frames.size(), trace.size());
        }
    }
};

QTEST_MAIN(ExecutionTest)

#include "executiontest.moc"

