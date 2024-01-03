/*
  gdbinjector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    QString name() const override;
    bool launch(const QStringList &programAndArgs, const QString &probeDll,
                const QString &probeFunc, const QProcessEnvironment &env) override;
    bool attach(int pid, const QString &probeDll, const QString &probeFunc) override;

protected:
    void addFunctionBreakpoint(const QByteArray &function) override;
    void addMethodBreakpoint(const QByteArray &method) override;
    void clearBreakpoints() override;
    void printBacktrace() override;
    void loadSymbols(const QByteArray &library) override;
    void parseStandardError(const QByteArray &line) override;

private:
    void setupGdb();
    /** Certain configurations crash with auto-solib-add off. */
    bool supportsAutoSolibAddOff() const;
};
}

#endif // GAMMARAY_GDBINJECTOR_H
