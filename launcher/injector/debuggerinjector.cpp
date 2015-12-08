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

void DebuggerInjector::stop()
{
    if (m_process) {
        m_process->terminate();
        if (!m_process->waitForFinished(1000))
            m_process->kill(); // kill it softly
    }
    emit finished();
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
  emit stdoutMessage(m_process->readAllStandardOutput());
}

void DebuggerInjector::processFinished()
{
    mExitCode = m_process->exitCode();
    mExitStatus = m_process->exitStatus();
    if (!mManualError) {
      mProcessError = m_process->error();
      mErrorString = m_process->errorString();
    }
    emit attached();
}

void DebuggerInjector::readyReadStandardError()
{
  const QString error = m_process->readAllStandardError();
  std::cerr << qPrintable(error) << std::endl;
  emit stderrMessage(error);
}

bool DebuggerInjector::startDebugger(const QStringList& args, const QProcessEnvironment &env)
{
  m_process.reset(new QProcess);
  if (!env.isEmpty())
    m_process->setProcessEnvironment(env);
  connect(m_process.data(), SIGNAL(readyReadStandardError()),
          this, SLOT(readyReadStandardError()));
  connect(m_process.data(), SIGNAL(readyReadStandardOutput()),
          this, SLOT(readyReadStandardOutput()));
  connect(m_process.data(), SIGNAL(started()),
          this, SIGNAL(started()));
  connect(m_process.data(), SIGNAL(finished(int)),
          this, SLOT(processFinished()));
  m_process->setProcessChannelMode(QProcess::SeparateChannels);
  m_process->start(debuggerExecutable(), args);
  bool status = m_process->waitForStarted(-1);

  if (!status) {
    mExitCode = m_process->exitCode();
    mExitStatus = m_process->exitStatus();
    if (!mManualError) {
      mProcessError = m_process->error();
      mErrorString = m_process->errorString();
    }
  } else {
      emit started();
  }
  return status;
}

bool DebuggerInjector::selfTest()
{
  if (startDebugger(QStringList() << QStringLiteral("--version"))) {
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

bool DebuggerInjector::injectAndDetach(const QString &probeDll, const QString &probeFunc)
{
  Q_ASSERT(m_process);
  loadSymbols("dl");
  execCmd(QStringLiteral("call (void) dlopen(\"%1\", %2)").
          arg(probeDll).arg(RTLD_NOW).toUtf8());
  loadSymbols(probeDll.toUtf8());
  execCmd(QStringLiteral("call (void) %1()").arg(probeFunc).toUtf8());

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

  return true;
}

void DebuggerInjector::loadSymbols(const QByteArray& library)
{
  Q_UNUSED(library);
}
