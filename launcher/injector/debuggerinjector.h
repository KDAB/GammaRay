/*
  debuggerinjector.h

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

#ifndef GAMMARAY_DEBUGGERINJECTOR_H
#define GAMMARAY_DEBUGGERINJECTOR_H

#include "injector/abstractinjector.h"

namespace GammaRay {

/** Base class for debugger-based injectors. */
class DebuggerInjector : public AbstractInjector
{
  public:
    DebuggerInjector();
    ~DebuggerInjector();

    QString errorString();
    int exitCode();
    QProcess::ExitStatus exitStatus();
    QProcess::ProcessError processError();

  protected:
    int mExitCode;
    QProcess::ProcessError mProcessError;
    QProcess::ExitStatus mExitStatus;
    QString mErrorString;

  private:
};

}

#endif // GAMMARAY_DEBUGGERINJECTOR_H
