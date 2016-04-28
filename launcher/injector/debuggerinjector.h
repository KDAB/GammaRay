/*
  debuggerinjector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_DEBUGGERINJECTOR_H
#define GAMMARAY_DEBUGGERINJECTOR_H

#include "abstractinjector.h"

#include <QObject>
#include <QProcess>

namespace GammaRay {

/** Base class for debugger-based injectors. */
class DebuggerInjector : public AbstractInjector
{
  Q_OBJECT
  public:
    DebuggerInjector();
    ~DebuggerInjector();

    // The debugger executable location
    QString filePath() const;
    void setFilePath(const QString &filePath);

    void stop() Q_DECL_OVERRIDE;

    QProcess::ExitStatus exitStatus() Q_DECL_OVERRIDE;
    QProcess::ProcessError processError() Q_DECL_OVERRIDE;
    int exitCode() Q_DECL_OVERRIDE;
    QString errorString() Q_DECL_OVERRIDE;
    bool selfTest() Q_DECL_OVERRIDE;

  protected:
    /** Execute a raw command on the debugger. */
    virtual void execCmd(const QByteArray &cmd, bool waitForWritten = true);
    /** Turn off confirmations */
    virtual void disableConfirmations() = 0;
    /** Break in the function @p function, specify name without parenthesis. */
    virtual void addFunctionBreakpoint(const QByteArray &function) = 0;
    /** Break in the method @p method, specify name without parenthesis. */
    virtual void addMethodBreakpoint(const QByteArray &method) = 0;
    /** Clear all breakpoints */
    virtual void clearBreakpoints() = 0;
    /** Print current thread backtrace */
    virtual void printBacktrace() = 0;
    /** Load symbols for the given shared library. */
    virtual void loadSymbols(const QByteArray &library);

    /** Start the debugger with the given command line arguments. */
    bool startDebugger(const QStringList &args, const QProcessEnvironment &env = QProcessEnvironment());
    /** Add a breakpoint in common entry points and wait until they are hit. */
    void waitForMain();
    /** Given an interrupted process, this injects the probe and continues the process. */
    bool injectAndDetach(const QString &probeDll, const QString &probeFunc);

  protected slots:
    virtual void readyReadStandardError();
    virtual void readyReadStandardOutput();

  private slots:
    void processFinished();

  protected:
    QScopedPointer<QProcess> m_process;
    int mExitCode;
    QProcess::ProcessError mProcessError;
    QProcess::ExitStatus mExitStatus;
    QString m_filePath;
    QString mErrorString;
    bool mManualError;

  protected:
    enum Orientation {
      In,
      Out
    };

    void processLog(DebuggerInjector::Orientation orientation, bool isError, const QString &text);
};

}

#endif // GAMMARAY_DEBUGGERINJECTOR_H
