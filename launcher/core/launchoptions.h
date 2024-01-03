/*
  launchoptions.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_LAUNCHOPTIONS_H
#define GAMMARAY_LAUNCHOPTIONS_H

#include "gammaray_launcher_export.h"

#include <QHash>
#include <QSharedDataPointer>
#include <QStringList>

QT_BEGIN_NAMESPACE
class QVariant;
class QProcessEnvironment;
QT_END_NAMESPACE

namespace GammaRay {
class ProbeABI;
class LaunchOptionsPrivate;

/** @brief Describes the injection and probe options used for launching/attacing to a host process. */
class GAMMARAY_LAUNCHER_EXPORT LaunchOptions
{
public:
    LaunchOptions();
    LaunchOptions(const LaunchOptions &other);
    ~LaunchOptions();
    LaunchOptions &operator=(const LaunchOptions &other);

    enum UiMode
    {
        InProcessUi,
        OutOfProcessUi,
        NoUi
    };

    /** Returns @c true if this is valid and has launch arguments set. */
    bool isLaunch() const;

    /** Returns @c true if we are supposed to attach rather than start a new process. */
    bool isAttach() const;

    /** Returns @c true if no valid launch arguments or process id are set. */
    bool isValid() const;

    /** Generic key/value settings send to the probe. */
    void setProbeSetting(const QString &key, const QVariant &value);
    QHash<QByteArray, QByteArray> probeSettings() const;

    /** Program and command line arguments to launch. */
    void setLaunchArguments(const QStringList &args);
    QStringList launchArguments() const;
    /** Absolute path (as far as it can be determined) of the executable to launch.
     *  Only valid if isLaunch() returns @c true.
     */
    QString absoluteExecutablePath() const;

    /** Process id for the process to attach to. */
    void setPid(qint64 pid);
    qint64 pid() const;

    /** UI mode. */
    UiMode uiMode() const;
    void setUiMode(UiMode mode);

    /** Injector type. */
    QString injectorType() const;
    void setInjectorType(const QString &injectorType);

    /** Injector executable override. */
    QString injectorTypeExecutableOverride() const;
    void setInjectorTypeExecutableOverride(const QString &filePath);

    /** Probe ABI. */
    ProbeABI probeABI() const;
    void setProbeABI(const ProbeABI &abi);

    /** Full path to the probe being used. This overrides specifying a probe ABI and
     *  can be useful on non-standard installation layouts of the probes.
     */
    void setProbePath(const QString &path);
    QString probePath() const;

    /** Working directory for launching the target. */
    void setWorkingDirectory(const QString &path);
    QString workingDirectory() const;

    /** Process environment for the launched target. By default the environment of the launcher process is used. */
    void setProcessEnvironment(const QProcessEnvironment &env);
    QProcessEnvironment processEnvironment() const;

    /** execute this launch options with the given command-line launcher. */
    bool execute(const QString &launcherPath) const;

private:
    QSharedDataPointer<LaunchOptionsPrivate> d;
};
}

#endif // GAMMARAY_LAUNCHOPTIONS_H
