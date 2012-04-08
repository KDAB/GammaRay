/*
  styleinjector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_STYLEINJECTOR_H
#define GAMMARAY_STYLEINJECTOR_H

#include <injector/abstractinjector.h>

namespace GammaRay {

class StyleInjector : public GammaRay::AbstractInjector
{

public:
    StyleInjector();
    QString name() const {
      return QString("style");
    }
    virtual bool launch(const QStringList &programAndArgs,
                       const QString &probeDll, const QString &probeFunc);
    virtual bool selfTest();
    virtual int exitCode();
    virtual QProcess::ExitStatus exitStatus();
    virtual QProcess::ProcessError processError();
    virtual QString errorString();

  private:
    int mExitCode;
    QProcess::ProcessError mProcessError;
    QProcess::ExitStatus mExitStatus;
    QString mErrorString;
};

}

#endif // GAMMARAY_STYLEINJECTOR_H
