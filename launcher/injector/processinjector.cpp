/*
  processinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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


#include "processinjector.h"
#include "interactiveprocess.h"

using namespace GammaRay;

ProcessInjector::ProcessInjector() :
  mExitCode(-1),
  mProcessError(QProcess::UnknownError),
  mExitStatus(QProcess::NormalExit)
{
}

ProcessInjector::~ProcessInjector()
{
}

bool ProcessInjector::launchProcess(const QStringList& programAndArgs, const QProcessEnvironment& env)
{
  InteractiveProcess proc;
  proc.setProcessEnvironment(env);
  proc.setProcessChannelMode(QProcess::ForwardedChannels);

  QStringList args = programAndArgs;

  if (env.value("GAMMARAY_GDB").toInt()) {
    QStringList newArgs;
    newArgs << "gdb";
#ifndef Q_OS_MAC
    newArgs << "--eval-command" << "run";
#endif
    newArgs << "--args";
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

  if (mProcessError == QProcess::FailedToStart) {
    mErrorString.prepend(QString("Could not start '%1': ").arg(program));
  }

  return mExitCode == EXIT_SUCCESS && mExitStatus == QProcess::NormalExit
          && mProcessError == QProcess::UnknownError;
}

int ProcessInjector::exitCode()
{
  return mExitCode;
}

QProcess::ExitStatus ProcessInjector::exitStatus()
{
  return mExitStatus;
}

QProcess::ProcessError ProcessInjector::processError()
{
  return mProcessError;
}

QString ProcessInjector::errorString()
{
  return mErrorString;
}
