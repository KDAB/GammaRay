/*
  windllinjector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Patrick Spendrin <ps_ml@gmx.de>

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

#include "injector/abstractinjector.h"

#include <qglobal.h>

#ifdef Q_OS_WIN

#include <QStringList>
#include <windows.h>

namespace GammaRay {

class WinDllInjector : public AbstractInjector
{
  public:
    WinDllInjector();
    QString name() const {
      return QString("windll");
    }
    virtual bool launch(const QStringList &programAndArgs,
                       const QString &probeDll, const QString &probeFunc);
    virtual bool attach(int pid, const QString &probeDll, const QString &probeFunc);
    virtual int exitCode();
    virtual QProcess::ExitStatus exitStatus();
    virtual QProcess::ProcessError processError();
    virtual QString errorString();

  private:
    int mExitCode;
    QProcess::ProcessError mProcessError;
    QProcess::ExitStatus mExitStatus;
    QString mErrorString;

    bool inject();
    bool inject2();
    HANDLE m_destProcess;
    HANDLE m_destThread;
    QString m_dllPath;
};

}

#endif

#endif // GAMMARAY_WINDLLINJECTOR_H
