/*
  preloadinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef Q_OS_WIN

#include <QProcess>
#include <cstdlib>
#include <QDebug>

using namespace GammaRay;

PreloadInjector::PreloadInjector() :
  mExitCode(-1),
  mProcessError(QProcess::UnknownError),
  mExitStatus(QProcess::NormalExit)
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
  //env.insert("DYLD_PRINT_ENV", QLatin1String("1"));
  //env.insert("DYLD_PRINT_LIBRARIES", QLatin1String("1"));
  //env.insert("DYLD_PRINT_INITIALIZERS", QLatin1String("1"));
  env.insert("GAMMARAY_UNSET_DYLD", "1");
#else
  env.insert("LD_PRELOAD", probeDll);
  env.insert("GAMMARAY_UNSET_PRELOAD", "1");
#endif
  InteractiveProcess proc;
  proc.setProcessEnvironment(env);
  proc.setProcessChannelMode(QProcess::ForwardedChannels);

  QStringList args = programAndArgs;

  if (env.value("GAMMARAY_GDB").toInt()) {
    QStringList newArgs;
    newArgs << "gdb" << "--eval-command" << "run" << "--args";
    newArgs += args;
    args = newArgs;
  } else if (env.value("GAMMARAY_MEMCHECK").toInt()) {
    QStringList newArgs;
    newArgs << "valgrind"
            << "--tool=memcheck"
            << "--track-origins=yes"
            << "--num-callers=25"
            << "--leak-check=full";
    newArgs += args;
    args = newArgs;
  } else if (env.value("GAMMARAY_HELGRIND").toInt()) {
    QStringList newArgs;
    newArgs << "valgrind" << "--tool=helgrind";
    newArgs += args;
    args = newArgs;
  }

  const QString program = args.takeFirst();
  proc.start(program, args);
  proc.waitForFinished(-1);

  mExitCode = proc.exitCode();
  mProcessError = proc.error();
  mExitStatus = proc.exitStatus();
  mErrorString = proc.errorString();

  return mExitCode == EXIT_SUCCESS && mExitStatus == QProcess::NormalExit
          && mProcessError == QProcess::UnknownError;
}

int PreloadInjector::exitCode()
{
  return mExitCode;
}

QProcess::ProcessError PreloadInjector::processError()
{
  return mProcessError;
}

QProcess::ExitStatus PreloadInjector::exitStatus()
{
  return mExitStatus;
}

QString PreloadInjector::errorString()
{
  return mErrorString;
}

#endif
