/*
  gdbinjector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_GDBINJECTOR_H
#define GAMMARAY_GDBINJECTOR_H

#include "injector/debuggerinjector.h"

namespace GammaRay {
class GdbInjector : public DebuggerInjector
{
    Q_OBJECT
public:
    explicit GdbInjector(const QString &executableOverride = QString());
    QString name() const Q_DECL_OVERRIDE;
    bool launch(const QStringList &programAndArgs, const QString &probeDll,
                const QString &probeFunc, const QProcessEnvironment &env) Q_DECL_OVERRIDE;
    bool attach(int pid, const QString &probeDll, const QString &probeFunc) Q_DECL_OVERRIDE;

protected:
    void disableConfirmations() Q_DECL_OVERRIDE;
    void addFunctionBreakpoint(const QByteArray &function) Q_DECL_OVERRIDE;
    void addMethodBreakpoint(const QByteArray &method) Q_DECL_OVERRIDE;
    void clearBreakpoints() Q_DECL_OVERRIDE;
    void printBacktrace() Q_DECL_OVERRIDE;
    void loadSymbols(const QByteArray &library) Q_DECL_OVERRIDE;

private slots:
    void readyReadStandardError() Q_DECL_OVERRIDE;
    void readyReadStandardOutput() Q_DECL_OVERRIDE;
};
}

#endif // GAMMARAY_GDBINJECTOR_H
