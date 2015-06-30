/*
  processinjector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_PROCESSINJECTOR_H
#define GAMMARAY_PROCESSINJECTOR_H

#include "abstractinjector.h"

namespace GammaRay {

/** Convenience base class for injectors using QProcess
 *  to launch the target process.
 *  Provides support for using gdb or valgrind for debugging the target.
 */
class ProcessInjector : public AbstractInjector
{
  public:
    ProcessInjector();
    ~ProcessInjector();

    int exitCode() Q_DECL_OVERRIDE;
    QProcess::ExitStatus exitStatus() Q_DECL_OVERRIDE;
    QProcess::ProcessError processError() Q_DECL_OVERRIDE;
    QString errorString() Q_DECL_OVERRIDE;

  protected:
    bool launchProcess(const QStringList &programAndArgs, const QProcessEnvironment &env);
    int mExitCode;
    QString mErrorString;

  private:
    QProcess::ProcessError mProcessError;
    QProcess::ExitStatus mExitStatus;
};
}

#endif // GAMMARAY_PROCESSINJECTOR_H
