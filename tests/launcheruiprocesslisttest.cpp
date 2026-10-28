/*
  launcheruiiptest.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Gleb Popov <arrowd@FreeBSD.org>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>

#include <launcher/ui/processlist.h>

#include <QTest>

#include <algorithm>

#ifdef Q_OS_LINUX
#include <sys/prctl.h>
#endif

using namespace GammaRay;

class LauncherUiProcessListTest : public QObject
{
    Q_OBJECT
private slots:

    static void testProcessList()
    {
        ProcDataList previousList;
        ProcDataList newList = processList(previousList);

        QVERIFY(!newList.isEmpty());

        const auto &proc = newList[0];

#ifndef Q_OS_WIN
        QVERIFY(proc.ppid != 0);
        QVERIFY(!proc.state.isEmpty());
        QVERIFY(!proc.user.isEmpty());
#endif

        QVERIFY(!proc.name.isEmpty());
    }

#ifdef Q_OS_LINUX
    // the name in /proc/<pid>/stat can contain spaces
    static void testNameWithSpaces()
    {
        char oldName[16] = {};
        prctl(PR_GET_NAME, oldName);
        prctl(PR_SET_NAME, "gamma ray (test)");
        const auto procs = processList({});
        prctl(PR_SET_NAME, oldName);

        const auto it = std::find_if(procs.cbegin(), procs.cend(), [](const ProcData &proc) {
            return proc.ppid == QCoreApplication::applicationPid();
        });
        QVERIFY(it != procs.cend());
        // a single letter, e.g. "S" while waiting for the list
        QCOMPARE(it->state.size(), 1);
    }
#endif
};

QTEST_MAIN(LauncherUiProcessListTest)

#include "launcheruiprocesslisttest.moc"
