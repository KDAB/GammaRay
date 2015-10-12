/*
  windllinjector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Patrick Spendrin <ps_ml@gmx.de>

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

#ifndef GAMMARAY_WINDLLINJECTOR_H
#define GAMMARAY_WINDLLINJECTOR_H

#include "abstractinjector.h"

#include <qglobal.h>

#ifdef Q_OS_WIN

#include <QStringList>
#include <windows.h>

namespace GammaRay {
class FinishWaiter;
class WinDllInjector : public AbstractInjector
{
  Q_OBJECT
  public:
    WinDllInjector();
    ~WinDllInjector();
    QString name() const Q_DECL_OVERRIDE;
    bool launch(const QStringList &programAndArgs,
                const QString &probeDll, const QString &probeFunc,
                const QProcessEnvironment &env) Q_DECL_OVERRIDE;
    bool attach(int pid, const QString &probeDll, const QString &probeFunc) Q_DECL_OVERRIDE;
    int exitCode() Q_DECL_OVERRIDE;
    QProcess::ExitStatus exitStatus() Q_DECL_OVERRIDE;
    QProcess::ProcessError processError() Q_DECL_OVERRIDE;
    QString errorString() Q_DECL_OVERRIDE;
    void stop();
  private:
    int mExitCode;
    QProcess::ProcessError mProcessError;
    QProcess::ExitStatus mExitStatus;
    QString mErrorString;

    void inject();
    HANDLE m_destProcess;
    HANDLE m_destThread;
    QString m_dllPath;
    FinishWaiter *m_injectThread;
    friend class FinishWaiter;
};

}

#endif

#endif // GAMMARAY_WINDLLINJECTOR_H
