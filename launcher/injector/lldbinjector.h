/*
  lldbinjector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_LLDBINJECTOR_H
#define GAMMARAY_LLDBINJECTOR_H

#include "injector/debuggerinjector.h"

namespace GammaRay {

class LldbInjector : public DebuggerInjector
{
  public:
    LldbInjector();
    ~LldbInjector();

    QString name() const;
    bool launch(const QStringList& programAndArgs, const QString& probeDll, const QString& probeFunc);
    bool attach(int pid, const QString& probeDll, const QString& probeFunc);

  protected:
    QString debuggerExecutable() const;
    void execCmd(const QByteArray& cmd, bool waitForWritten = true);
    void addFunctionBreakpoint(const QByteArray& function);
    void addMethodBreakpoint(const QByteArray& method);
};
}

#endif // GAMMARAY_LLDBINJECTOR_H
