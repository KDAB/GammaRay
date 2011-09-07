/*
  preloadinjector.h

  This file is part of Endoscope, the Qt application inspection and
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

#ifndef ENDOSCOPE_PRELOADINJECTOR_H
#define ENDOSCOPE_PRELOADINJECTOR_H

#include "abstractinjector.h"
#include <qglobal.h>

#ifndef Q_OS_WIN

namespace Endoscope {

class PreloadInjector : public AbstractInjector
{
  public:
    PreloadInjector();
    QString name() const {
      return QString("preload");
    }
    virtual int launch(const QStringList &programAndArgs,
                       const QString &probeDll, const QString &probeFunc);
    virtual int exitCode();
    virtual QProcess::ExitStatus exitStatus();
    virtual QProcess::ProcessError processError();

  private:
    int mExitCode;
    QProcess::ProcessError mProcessError;
    QProcess::ExitStatus mExitStatus;
};

}

#endif

#endif // ENDOSCOPE_PRELOADINJECTOR_H
