/*
  windllinjector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Patrick Spendrin <patrick.spendrin@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_WINDLLINJECTOR_H
#define GAMMARAY_WINDLLINJECTOR_H

#include "abstractinjector.h"

#include <qglobal.h>
#include <QStringList>
#include <qt_windows.h>

namespace GammaRay {
class FinishWaiter;
class WinDllInjector : public AbstractInjector
{
    Q_OBJECT
public:
    WinDllInjector();
    ~WinDllInjector();
    QString name() const override;
    bool launch(const QStringList &programAndArgs, const QString &probeDll,
                const QString &probeFunc, const QProcessEnvironment &env) override;
    bool attach(int pid, const QString &probeDll, const QString &probeFunc) override;
    int exitCode() override;
    QProcess::ExitStatus exitStatus() override;
    QProcess::ProcessError processError() override;
    QString errorString() override;
    void stop() override;
private slots:
    void processExited(int exitCode);

private:
    QString fixProbeDllPath(const QString &probeDll);

    int mExitCode;
    QProcess::ProcessError mProcessError;
    QProcess::ExitStatus mExitStatus;
    QString mErrorString;

    HANDLE m_destProcess;
    FinishWaiter *m_injectThread;
    friend class FinishWaiter;
};
}

#endif // GAMMARAY_WINDLLINJECTOR_H
