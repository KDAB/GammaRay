/*
  debuggerinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "debuggerinjector.h"

#include <QProcess>

#include <iostream>
#include <dlfcn.h>
#include <stdlib.h>

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

void DebuggerInjector::waitForMain()
{
  addFunctionBreakpoint("main");
  execCmd("run");

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  loadSymbols("QtCore");
#else
  loadSymbols("Qt5Core");
#endif
  addMethodBreakpoint("QCoreApplication::exec");
  execCmd("continue");
}

int DebuggerInjector::injectAndDetach(const QString &probeDll, const QString &probeFunc)
{
  Q_ASSERT(m_process);
  loadSymbols("dl");
  execCmd(QString::fromLatin1("call (void) dlopen(\"%1\", %2)").
          arg(probeDll).arg(RTLD_NOW).toUtf8());
  loadSymbols(probeDll.toUtf8());
  execCmd(QString::fromLatin1("call (void) %1()").arg(probeFunc).toUtf8());

  if (qgetenv("GAMMARAY_UNITTEST") != "1") {
    execCmd("detach");
    execCmd("quit");
  } else {
    // delete all breakpoints before we continue, so we don't hit another one and abort there
    execCmd("delete");
    execCmd("continue");
    // if we hit a crash or anything, print backtrace and quit
    execCmd("backtrace", false);
    execCmd("quit", false);
  }

  m_process->waitForFinished(-1);

  mExitCode = m_process->exitCode();
  mExitStatus = m_process->exitStatus();
  if (!mManualError) {
    mProcessError = m_process->error();
    mErrorString = m_process->errorString();
  }

  return mExitCode == EXIT_SUCCESS && mExitStatus == QProcess::NormalExit;
}

void DebuggerInjector::loadSymbols(const QByteArray& library)
{
  Q_UNUSED(library);
}
