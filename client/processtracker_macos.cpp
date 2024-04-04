/*
  processtracker_macos.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

// A said crossplatform way (using ptrace) is available at
// https://forum.juce.com/t/detecting-if-a-process-is-being-run-under-a-debugger/2098

#include "processtracker_macos.h"

#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/sysctl.h>

using namespace GammaRay;

ProcessTrackerBackendMacOS::ProcessTrackerBackendMacOS(QObject *parent)
    : GammaRay::ProcessTrackerBackend(parent)
{
}

void ProcessTrackerBackendMacOS::checkProcess(qint64 pid)
{
    GammaRay::ProcessTrackerInfo pinfo(pid);
    int status;
    int mib[4];
    struct kinfo_proc info;
    size_t size;

    // Initialize the flags so that, if sysctl fails for some bizarre
    // reason, we get a predictable result.
    info.kp_proc.p_flag = 0;

    // Initialize mib, which tells sysctl the info we want, in this case
    // we're looking for information about a specific process ID.
    mib[0] = CTL_KERN;
    mib[1] = KERN_PROC;
    mib[2] = KERN_PROC_PID;
    mib[3] = pid;

    // Call sysctl.
    size = sizeof(info);
    status = sysctl(mib, sizeof(mib) / sizeof(*mib), &info, &size, nullptr, 0);

    if (status == 0) {
        // We're being debugged if the P_TRACED flag is set.
        pinfo.traced = (info.kp_proc.p_flag & P_TRACED) != 0;

        switch (info.kp_proc.p_stat) {
        case SRUN:
            pinfo.state = GammaRay::ProcessTracker::Running;
            break;
        case SSTOP:
        case SSLEEP:
            pinfo.state = GammaRay::ProcessTracker::Suspended;
            break;
        default:
            break;
        }
    } else {
        qWarning("%s: Can not sysctl.", Q_FUNC_INFO);
    }

    emit processChecked(pinfo);
}
