/*
  preloadinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "preloadinjector.h"

#include "interactiveprocess.h"
#include "preloadcheck.h"

#ifndef Q_OS_WIN

#include <QProcess>
#include <QDebug>

#include <cstdlib>

using namespace GammaRay;

PreloadInjector::PreloadInjector() : ProcessInjector()
{
}

bool PreloadInjector::launch(const QStringList &programAndArgs,
                            const QString &probeDll,
                            const QString &probeFunc)
{
  Q_UNUSED(probeFunc);

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#ifdef Q_OS_MAC
  env.insert("DYLD_FORCE_FLAT_NAMESPACE", QLatin1String("1"));
  env.insert("DYLD_INSERT_LIBRARIES", probeDll);
  env.insert("GAMMARAY_UNSET_DYLD", "1");
#else
  env.insert("LD_PRELOAD", probeDll);
  env.insert("GAMMARAY_UNSET_PRELOAD", "1");

  PreloadCheck check;
  const bool success = check.test("qt_startup_hook");
#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0) // before 5.4 this is fatal, after that we have the built-in hooks and DLL initialization as an even better way
  if (!success) {
    mExitCode = 1;
    mErrorString = check.errorString();
    return false;
  }
#else
  Q_UNUSED(success);
#endif

#endif

  return launchProcess(programAndArgs, env);
}

#endif
