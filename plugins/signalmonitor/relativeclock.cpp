/*
  relativeclock.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "relativeclock.h"

#ifdef Q_OS_LINUX
#include <QCoreApplication>
#include <QFileInfo>
#endif // Q_OS_LINUX

#include <QDateTime>

using namespace GammaRay;

static qint64 appStartTime()
{
#ifdef Q_OS_LINUX

    // On Linux the application start time can be read by procfs.
    const QString &self = QStringLiteral("/proc/%1").arg(qApp->applicationPid());
    return QFileInfo(self).lastModified().toMSecsSinceEpoch();

#else // !Q_OS_LINUX

    // On other platforms this is a rough estimation if called early.
    return QDateTime::currentMSecsSinceEpoch();

#endif // !Q_OS_LINUX
}

const RelativeClock *RelativeClock::sinceAppStart()
{
    static const RelativeClock clock(appStartTime());
    return &clock;
}

qint64 RelativeClock::currentMSecsSinceEpoch()
{
    return QDateTime::currentMSecsSinceEpoch();
}
