/*
  abstractinjector.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_ABSTRACTINJECTOR_H
#define GAMMARAY_ABSTRACTINJECTOR_H

#include <launcher/core/probeabi.h>

#include <QProcess>
#include <QSharedPointer>

QT_BEGIN_NAMESPACE
class QProcessEnvironment;
class QString;
class QStringList;
QT_END_NAMESPACE

namespace GammaRay {
class AbstractInjector : public QObject
{
    Q_OBJECT

public:
    using Ptr = QSharedPointer<AbstractInjector>;
    ~AbstractInjector() override;

    /**
     * Injector Name
     */
    virtual QString name() const = 0;

    /**
     * Launch the application @p program and inject @p probeDll and call @p probeFunc on it.
     * Assuming the launcher supports this, @p env allows you to customize the environment
     * variables of the started application.
     *
     * @return True if the launch succeeded, false otherwise.
     */
    virtual bool launch(const QStringList &programAndArgs, const QString &probeDll,
                        const QString &probeFunc, const QProcessEnvironment &env);

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

    /** Attempts to stop a running process. */
    virtual void stop() = 0;

    /** Returns the working directory supposed to be used for launching. */
    QString workingDirectory() const;
    /** Set working directory for launching. */
    void setWorkingDirectory(const QString &path);

    /*! Returns the ABI of the target. */
    ProbeABI targetAbi() const;
    /*! Sets the ABI of the target. */
    void setTargetAbi(const ProbeABI &abi);

signals:
    void started();
    void finished();
    void attached();

    void stdoutMessage(const QString &message);
    void stderrMessage(const QString &message);

private:
    QString m_workingDir;
    ProbeABI m_targetAbi;
};
}

#endif // ABSTRACTINJECTOR_H
