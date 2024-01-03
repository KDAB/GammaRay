/*
  launchoptions.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "launchoptions.h"
#include "probeabi.h"

#include <QVariant>
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

#include <iostream>

namespace GammaRay {
class LaunchOptionsPrivate : public QSharedData
{
public:
    LaunchOptionsPrivate()
        : env(QProcessEnvironment::systemEnvironment())
    {
    }

    QStringList launchArguments;
    QString injectorType;
    QString injectorTypeExecutableOverride;
    ProbeABI probeABI;
    qint64 pid = -1;
    LaunchOptions::UiMode uiMode = LaunchOptions::OutOfProcessUi;
    QHash<QByteArray, QByteArray> probeSettings;
    QProcessEnvironment env;
    QString workingDir;
};
}

using namespace GammaRay;

LaunchOptions::LaunchOptions()
    : d(new LaunchOptionsPrivate)
{
}

LaunchOptions::~LaunchOptions() = default;

LaunchOptions::LaunchOptions(const LaunchOptions &) = default;

LaunchOptions &LaunchOptions::operator=(const LaunchOptions &) = default;

bool LaunchOptions::isLaunch() const
{
    return !d->launchArguments.isEmpty();
}

bool LaunchOptions::isAttach() const
{
    return pid() > 0;
}

bool LaunchOptions::isValid() const
{
    return isLaunch() != isAttach();
}

QStringList LaunchOptions::launchArguments() const
{
    return d->launchArguments;
}

void LaunchOptions::setLaunchArguments(const QStringList &args)
{
    d->launchArguments = args;
    Q_ASSERT(d->pid <= 0 || d->launchArguments.isEmpty());
}

QString LaunchOptions::absoluteExecutablePath() const
{
    if (d->launchArguments.isEmpty())
        return QString();

    QString path = d->launchArguments.first();
    const QFileInfo fi(path);
    if (fi.isFile() && fi.isExecutable())
        return path;
    path = QStandardPaths::findExecutable(d->launchArguments.first());
    if (!path.isEmpty())
        return path;

    return d->launchArguments.first();
}

qint64 LaunchOptions::pid() const
{
    return d->pid;
}

void LaunchOptions::setPid(qint64 pid)
{
    d->pid = pid;
    Q_ASSERT(d->pid <= 0 || d->launchArguments.isEmpty());
}

LaunchOptions::UiMode LaunchOptions::uiMode() const
{
    return d->uiMode;
}

void LaunchOptions::setUiMode(LaunchOptions::UiMode mode)
{
    d->uiMode = mode;
    setProbeSetting(QStringLiteral("InProcessUi"), mode == InProcessUi);
}

QString LaunchOptions::injectorType() const
{
    return d->injectorType;
}

void LaunchOptions::setInjectorType(const QString &injectorType)
{
    d->injectorType = injectorType;
}

QString LaunchOptions::injectorTypeExecutableOverride() const
{
    return d->injectorTypeExecutableOverride;
}

void LaunchOptions::setInjectorTypeExecutableOverride(const QString &filePath)
{
    d->injectorTypeExecutableOverride = filePath;
}

ProbeABI LaunchOptions::probeABI() const
{
    return d->probeABI;
}

void LaunchOptions::setProbeABI(const ProbeABI &abi)
{
    d->probeABI = abi;
}

void LaunchOptions::setProcessEnvironment(const QProcessEnvironment &env)
{
    d->env = env;
}

QProcessEnvironment LaunchOptions::processEnvironment() const
{
    QProcessEnvironment env = d->env;
    for (auto it = d->probeSettings.constBegin(); it != d->probeSettings.constEnd(); ++it)
        env.insert("GAMMARAY_" + it.key(), it.value());
    return env;
}

void LaunchOptions::setProbePath(const QString &path)
{
    setProbeSetting(QStringLiteral("ProbePath"), path);
}

QString LaunchOptions::probePath() const
{
    return d->probeSettings.value("ProbePath");
}

void LaunchOptions::setWorkingDirectory(const QString &path)
{
    d->workingDir = path;
}

QString LaunchOptions::workingDirectory() const
{
    return d->workingDir;
}

void LaunchOptions::setProbeSetting(const QString &key, const QVariant &value)
{
    QByteArray v;
    switch (value.type()) {
    case QVariant::String:
        v = value.toString().toUtf8();
        break;
    case QVariant::Bool:
        v = value.toBool() ? "true" : "false";
        break;
    case QVariant::Int:
    case QVariant::UInt:
    case QVariant::LongLong:
    case QVariant::ULongLong:
        v = QByteArray::number(value.toInt());
        break;
    default:
        qFatal("unsupported probe settings type");
    }

    d->probeSettings.insert(key.toUtf8(), v);
}

QHash<QByteArray, QByteArray> LaunchOptions::probeSettings() const
{
    return d->probeSettings;
}

bool LaunchOptions::execute(const QString &launcherPath) const
{
    Q_ASSERT(!launcherPath.isEmpty());
    Q_ASSERT(isValid());

    QStringList args;
    switch (uiMode()) {
    case InProcessUi:
        args.push_back(QStringLiteral("--inprocess"));
        break;
    case OutOfProcessUi:
        args.push_back(QStringLiteral("--no-inprocess"));
        break;
    case NoUi:
        args.push_back(QStringLiteral("--inject-only"));
        break;
    }

    if (d->probeABI.isValid()) {
        args.push_back(QStringLiteral("--probe"));
        args.push_back(d->probeABI.id());
    }

    if (d->probeSettings.contains("ServerAddress")) {
        args.push_back(QStringLiteral("--listen"));
        args.push_back(d->probeSettings.value("ServerAddress"));
    }
    if (d->probeSettings.value("RemoteAccessEnabled", "true") == "false")
        args.push_back(QStringLiteral("--no-listen"));

    if (isAttach()) {
        args.push_back(QStringLiteral("--pid"));
        args.push_back(QString::number(pid()));
    } else {
        args += launchArguments();
    }

    auto workingDir = d->workingDir;
    if (workingDir.isEmpty()) {
        workingDir = QDir::currentPath();
    }

    std::cout << "Detaching: " << qPrintable(launcherPath) << " " << qPrintable(args.join(" ")) << "\n"
              << "Working Directory: " << qPrintable(workingDir)
              << std::endl;

    return QProcess::startDetached(launcherPath, args, workingDir);
}
