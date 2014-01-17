/*
  debuggerinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "debuggerinjector.h"

#include <QProcess>

#include <iostream>

using namespace GammaRay;

DebuggerInjector::DebuggerInjector() :
  mExitCode(-1),
  mProcessError(QProcess::UnknownError),
  mExitStatus(QProcess::NormalExit),
  mManualError(false)
{
}

DebuggerInjector::~DebuggerInjector()
{
}

QString DebuggerInjector::errorString()
{
  return mErrorString;
}

int DebuggerInjector::exitCode()
{
  return mExitCode;
}

QProcess::ExitStatus DebuggerInjector::exitStatus()
{
  return mExitStatus;
}

QProcess::ProcessError DebuggerInjector::processError()
{
  return mProcessError;
}

void DebuggerInjector::readyReadStandardOutput()
{
}

void DebuggerInjector::readyReadStandardError()
{
  const QString error = m_process->readAllStandardError();
  std::cerr << qPrintable(error) << std::endl;
}

bool DebuggerInjector::startDebugger(const QStringList& args)
{
  m_process.reset(new QProcess);
  connect(m_process.data(), SIGNAL(readyReadStandardError()),
          this, SLOT(readyReadStandardError()));
  connect(m_process.data(), SIGNAL(readyReadStandardOutput()),
          this, SLOT(readyReadStandardOutput()));
  m_process->setProcessChannelMode(QProcess::SeparateChannels);
  m_process->start(debuggerExecutable(), args);
  bool status = m_process->waitForStarted(-1);

  mExitCode = m_process->exitCode();
  mExitStatus = m_process->exitStatus();
  if (!mManualError) {
    mProcessError = m_process->error();
    mErrorString = m_process->errorString();
  }

  return status;
}

bool DebuggerInjector::selfTest()
{
  if (startDebugger(QStringList() << QLatin1String("--version"))) {
    return m_process->waitForFinished(-1);
  }
  return false;
}
