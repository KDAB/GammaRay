/*
  lldbinjector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_LLDBINJECTOR_H
#define GAMMARAY_LLDBINJECTOR_H

#include "injector/debuggerinjector.h"

namespace GammaRay {
class LldbInjector : public DebuggerInjector
{
    Q_OBJECT
public:
    explicit LldbInjector(const QString &executableOverride = QString());
    bool selfTest() override;
    QString name() const override;
    bool launch(const QStringList &programAndArgs, const QString &probeDll,
                const QString &probeFunc, const QProcessEnvironment &env) override;
    bool attach(int pid, const QString &probeDll, const QString &probeFunc) override;

protected:
    void disableConfirmations();
    void addFunctionBreakpoint(const QByteArray &function) override;
    void addMethodBreakpoint(const QByteArray &method) override;
    void clearBreakpoints() override;
    void printBacktrace() override;
    void parseStandardError(const QByteArray &line) override;

private:
    bool m_scriptSupportIsRequired;
};
}

#endif // GAMMARAY_LLDBINJECTOR_H
