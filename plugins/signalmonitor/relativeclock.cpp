/*
  relativeclock.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Mathias Hasselmann <mathias.hasselmann@kdab.com>

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
  const QString &self = QString::fromLatin1("/proc/%1").arg(qApp->applicationPid());
  return QFileInfo(self).lastModified().toMSecsSinceEpoch();

#else // !Q_OS_LINUX

  // On other platforms this is a rough estimation if called early.
  return QDateTime::currentMSecsSinceEpoch();

#endif // !Q_OS_LINUX
}

const RelativeClock* RelativeClock::sinceAppStart()
{
  static const RelativeClock clock(appStartTime());
  return &clock;
}

qint64 RelativeClock::currentMSecsSinceEpoch()
{
  return QDateTime::currentMSecsSinceEpoch();
}
