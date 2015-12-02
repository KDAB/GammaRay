/*
  preloadinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "preloadinjector.h"
#include "preloadcheck.h"

#include <probeabidetector.h>

#ifndef Q_OS_WIN

#include <QProcess>
#include <QDebug>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QStandardPaths>
#endif

#include <cstdlib>

using namespace GammaRay;

PreloadInjector::PreloadInjector() : ProcessInjector()
{
}

QString PreloadInjector::name() const
{
  return QStringLiteral("preload");
}

bool PreloadInjector::launch(const QStringList &programAndArgs,
                            const QString &probeDll,
                            const QString &probeFunc,
                            const QProcessEnvironment &e)
{
  Q_UNUSED(probeFunc);

  QProcessEnvironment env(e);
#ifdef Q_OS_MAC
  env.insert(QStringLiteral("DYLD_FORCE_FLAT_NAMESPACE"), QStringLiteral("1"));
  env.insert(QStringLiteral("DYLD_INSERT_LIBRARIES"), probeDll);
  env.insert(QStringLiteral("GAMMARAY_UNSET_DYLD"), QStringLiteral("1"));
#else
  env.insert(QStringLiteral("LD_PRELOAD"), probeDll);
  env.insert(QStringLiteral("GAMMARAY_UNSET_PRELOAD"), QStringLiteral("1"));

  auto exePath = programAndArgs.first();
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  exePath = QStandardPaths::findExecutable(exePath);
#endif

  ProbeABIDetector abiDetector;
  const auto qtCorePath = abiDetector.qtCoreForExecutable(exePath);
  PreloadCheck check;
  const bool success = check.test(qtCorePath, QStringLiteral("qt_startup_hook"));
#if QT_VERSION < QT_VERSION_CHECK(5, 4, 0) // before 5.4 this is fatal, after that we have the built-in hooks and DLL initialization as an even better way
  if (!success  && !qtCorePath.isEmpty()) {
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
