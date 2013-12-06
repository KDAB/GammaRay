/*
  gdbinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>
#include "gdbinjector.h"

#include <QDebug>
#include <QProcess>
#include <QStringList>
#include <QCoreApplication>

#include <dlfcn.h>

using namespace GammaRay;

static QTextStream cout(stdout);
static QTextStream cerr(stderr);

GdbInjector::GdbInjector() :
  mManualError(false),
  mExitCode(-1),
  mProcessError(QProcess::UnknownError),
  mExitStatus(QProcess::NormalExit)
{
}

bool GdbInjector::launch(const QStringList &programAndArgs,
                        const QString &probeDll, const QString &probeFunc)
{
  QStringList gdbArgs;
  gdbArgs.push_back(QLatin1String("--args"));
  gdbArgs.append(programAndArgs);

  if (!startGdb(gdbArgs)) {
    return -1;
  }

  execGdbCmd("break main");
  execGdbCmd("run");
  execGdbCmd("sha QtCore");
  // either this
  addBreakpoint("QCoreApplication::exec");
  // or this for unit tests should hit
  addBreakpoint("QTest::qExec");
  execGdbCmd("continue");

  return injectAndDetach(probeDll, probeFunc);
}

bool GdbInjector::attach(int pid, const QString &probeDll, const QString &probeFunc)
{
  Q_ASSERT(pid > 0);
  if (!startGdb(QStringList() << QLatin1String("-pid") << QString::number(pid))) {
    return false;
  }
  return injectAndDetach(probeDll, probeFunc);
}

bool GdbInjector::startGdb(const QStringList &args)
{
  m_process.reset(new QProcess);
  connect(m_process.data(), SIGNAL(readyReadStandardError()),
          this, SLOT(readyReadStandardError()));
  connect(m_process.data(), SIGNAL(readyReadStandardOutput()),
          this, SLOT(readyReadStandardOutput()));
  m_process->setProcessChannelMode(QProcess::SeparateChannels);
  m_process->start(QLatin1String("gdb"), args);
  bool status = m_process->waitForStarted(-1);

  mExitCode = m_process->exitCode();
  mExitStatus = m_process->exitStatus();
  if (!mManualError) {
    mProcessError = m_process->error();
    mErrorString = m_process->errorString();
  }

  return status;
}

bool GdbInjector::injectAndDetach(const QString &probeDll, const QString &probeFunc)
{
  Q_ASSERT(m_process);
#ifndef Q_OS_MAC
  execGdbCmd("sha dl");
#endif
  execGdbCmd(qPrintable(QString::fromLatin1("call (void) dlopen(\"%1\", %2)").
                        arg(probeDll).arg(RTLD_NOW)));
#ifndef Q_OS_MAC
  execGdbCmd(qPrintable(QString::fromLatin1("sha %1").arg(probeDll)));
#endif
//  execGdbCmd(qPrintable(QString::fromLatin1("call (void) %1()").arg(probeFunc)));
  execGdbCmd(qPrintable(QString::fromLatin1("print %1").arg(probeFunc)));
  execGdbCmd("call (void) $()");

  if (qgetenv("GAMMARAY_UNITTEST") != "1") {
    execGdbCmd("detach");
    execGdbCmd("quit");
  } else {
    execGdbCmd("continue");
    // if we hit a crash or anything, print backtrace and quit
    execGdbCmd("backtrace", false);
    execGdbCmd("quit", false);
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

void GdbInjector::execGdbCmd(const QByteArray &cmd, bool waitForWritten)
{
  m_process->write(cmd + '\n');

  if (waitForWritten) {
    m_process->waitForBytesWritten(-1);
  }
}

int GdbInjector::exitCode()
{
  return mExitCode;
}

QProcess::ProcessError GdbInjector::processError()
{
  return mProcessError;
}

QProcess::ExitStatus GdbInjector::exitStatus()
{
  return mExitStatus;
}

QString GdbInjector::errorString()
{
  return mErrorString;
}

void GdbInjector::readyReadStandardError()
{
  const QString error = m_process->readAllStandardError();
  cerr << error << flush;

  if (error.startsWith(QLatin1String("Function \"main\" not defined."))) {
    mManualError = true;
    mErrorString = tr("The debuggee application is missing debug symbols which are required\n"
                      "for GammaRay's GDB injector. Please recompile the debuggee.\n\n"
                      "GDB error was: %1").arg(error);
  } else if (error.startsWith(QLatin1String("Can't find member of namespace, class, struct, or union named \"QCoreApplication::exec\""))) {
    mManualError = true;
    mErrorString = tr("Your QtCore library is missing debug symbols which are required\n"
                      "for GammaRay's GDB injector. Please install the required debug symbols.\n\n"
                      "GDB error was: %1").arg(error);
  }

  if (mManualError) {
    m_process->kill();
    disconnect(m_process.data(), SIGNAL(readyReadStandardError()), this, 0);
    disconnect(m_process.data(), SIGNAL(readyReadStandardOutput()), this, 0);
    mProcessError = QProcess::FailedToStart;
    return;
  }
}

void GdbInjector::readyReadStandardOutput()
{
  if (qgetenv("GAMMARAY_UNITTEST") == "1") {
    cout << m_process->readAllStandardOutput() << flush;
  }
}

void GdbInjector::addBreakpoint(const QByteArray &method)
{
#ifdef Q_OS_MAC
  execGdbCmd("break " + method + "()");
#else
  execGdbCmd("break " + method);
#endif
}

bool GdbInjector::selfTest()
{
  if (startGdb(QStringList() << QLatin1String("--version"))) {
    return m_process->waitForFinished(-1);
  }
  return false;
}

