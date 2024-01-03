/*
  processinjector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROCESSINJECTOR_H
#define GAMMARAY_PROCESSINJECTOR_H

#include "abstractinjector.h"
#include <QProcess>

namespace GammaRay {
/** Convenience base class for injectors using QProcess
 *  to launch the target process.
 *  Provides support for using gdb or valgrind for debugging the target.
 */
class ProcessInjector : public AbstractInjector
{
    Q_OBJECT
public:
    ProcessInjector();
    ~ProcessInjector() override;

    void stop() override;
    int exitCode() override;
    QProcess::ExitStatus exitStatus() override;
    QProcess::ProcessError processError() override;
    QString errorString() override;

protected:
    bool launchProcess(const QStringList &programAndArgs, const QProcessEnvironment &_env);
    int mExitCode;
    QString mErrorString;

private slots:
    void processFailed();
    void processFinished();
    void readStdOut();
    void readStdErr();

private:
    void stop_impl();

private:
    QProcess::ProcessError mProcessError;
    QProcess::ExitStatus mExitStatus;
    QProcess m_proc;
};
}

#endif // GAMMARAY_PROCESSINJECTOR_H
