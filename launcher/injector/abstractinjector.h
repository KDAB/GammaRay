/*
  abstractinjector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_ABSTRACTINJECTOR_H
#define GAMMARAY_ABSTRACTINJECTOR_H

#include <QProcess>
#include <QSharedPointer>

class QString;
class QStringList;

namespace GammaRay {

class AbstractInjector
{
  public:
    typedef QSharedPointer<AbstractInjector> Ptr;
    virtual ~AbstractInjector();

    /**
     * Injector Name
     */
    virtual QString name() const = 0;

    /**
     * Launch the application @p program and inject @p probeDll and call @p probeFunc on it.
     *
     * @return True if the launch succeeded, false otherwise.
     */
    virtual bool launch(const QStringList &programAndArgs,
                       const QString &probeDll, const QString &probeFunc);

    /**
     * Attach to the running application with process id @p pid
     * and inject @p probeDll and call @p probeFunc on it.
     *
     * @return True if attaching succeeded, false otherwise.
     */
    virtual bool attach(int pid, const QString &probeDll, const QString &probeFunc);

    /**
     * Return the exit code from the application launch or attach.
     */
    virtual int exitCode() = 0;

    /**
     * Return the QProcess::ExitStatus from the application launch or attach.
     */
    virtual QProcess::ExitStatus exitStatus() = 0;

    /**
     * Return the QProcess::ProcessError from the application launch or attach.
     */
    virtual QProcess::ProcessError processError() = 0;

    /**
     * @return Descriptional error message when launch/attach/self-test failed.
     */
    virtual QString errorString() = 0;

    /**
     * Perform tests to ensure the injector is operational, such as testing if
     * all necessary runtime dependencies are available.
     * @return @c true on success, @c false otherwise.
     * @note Make sure to set errorString() when returning @c false.
     */
    virtual bool selfTest();
};

}

#endif // ABSTRACTINJECTOR_H
