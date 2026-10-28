/*
  probeabidetector.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>

#include "probeabidetector.h"
#include "launcherlog.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QSysInfo>

#include <algorithm>

using namespace GammaRay;

ProbeABIDetector::ProbeABIDetector() = default;

static QString tr(const char *text)
{
    return QCoreApplication::translate("GammaRay::ProbeABIDetector", text);
}

ProbeABI ProbeABIDetector::abiForExecutable(const QString &path, QString *errorString) const
{
    const QString qtCore = qtCoreForExecutable(path);
    qCInfo(launcherLog) << "QtCore of" << path << "is" << qtCore;
    if (qtCore.isEmpty()) {
        const auto error = tr("No QtCore library found among the libraries %1 uses.").arg(path);
        qCInfo(launcherLog) << error;
        if (errorString)
            *errorString = error;
        return {};
    }
    return abiForQtCore(qtCore, errorString);
}

ProbeABI ProbeABIDetector::abiForProcess(qint64 pid, QString *errorString) const
{
    const QString qtCore = qtCoreForProcess(pid);
    qCInfo(launcherLog) << "QtCore of process" << pid << "is" << qtCore;
    if (qtCore.isEmpty()) {
        const auto error = tr("No QtCore library found in process %1. Either it doesn't use Qt (yet), "
                              "or its libraries can't be read, e.g. as it belongs to another user.")
                               .arg(pid);
        qCInfo(launcherLog) << error;
        if (errorString)
            *errorString = error;
        return {};
    }
    return abiForQtCore(qtCore, errorString);
}

ProbeABI ProbeABIDetector::abiForQtCore(const QString &path, QString *errorString) const
{
    QString error;
    ProbeABI abi;
    const QVector<ProbeABI> abis = abiForQtCore(path);
    if (!QFileInfo::exists(path)) {
        error = tr("QtCore library %1 not found.").arg(path);
    } else if (abis.isEmpty()) {
        error = tr("Could not read QtCore library %1.").arg(path);
    } else {
        // Assume you want to run the one your system cpu is about if it exists
        // It could be improved asking the user which of the multiple abis
        // they want to run
        // FIXME this is not necessarily true for processes, since the user could have
        // forced to run a particular abi if the OS supports so
        const auto it = std::find_if(abis.cbegin(), abis.cend(), [](const ProbeABI &abi) {
            return abi.architecture() == QSysInfo::currentCpuArchitecture();
        });
        abi = it != abis.cend() ? *it : abis.first();

        if (!abi.hasQtVersion())
            error = tr("Could not determine the Qt version of %1.").arg(path);
        else if (abi.architecture().isEmpty())
            error = tr("Unknown or unsupported CPU architecture of %1.").arg(path);
        else if (!abi.isValid())
            error = tr("Could not determine the compiler %1 was built with.").arg(path);
    }

    if (!error.isEmpty()) {
        qCInfo(launcherLog) << error;
        if (errorString)
            *errorString = error;
        return {};
    }
    qCInfo(launcherLog) << "Detected ABI" << abi.id() << "of" << path;
    return abi;
}

QVector<ProbeABI> ProbeABIDetector::abiForQtCore(const QString &path) const
{
    QFileInfo fi(path);
    if (!fi.exists())
        return {};

    auto it = m_abiForQtCoreCache.constFind(fi.canonicalFilePath());
    if (it != m_abiForQtCoreCache.constEnd())
        return it.value();

    const QVector<ProbeABI> abi = detectAbiForQtCore(fi.canonicalFilePath());
    m_abiForQtCoreCache.insert(fi.canonicalFilePath(), abi);
    return abi;
}

QString ProbeABIDetector::qtCoreFromLsof(qint64 pid)
{
    QString lsofExe;
    lsofExe = QStandardPaths::findExecutable(QStringLiteral("lsof"));
    // on OSX it's in sbin, which usually but not always is in PATH...
    if (lsofExe.isEmpty()) {
        lsofExe = QStandardPaths::findExecutable(QStringLiteral("lsof"),
                                                 QStringList() << QStringLiteral(
                                                     "/usr/sbin")
                                                               << QStringLiteral("/sbin"));
    }
    if (lsofExe.isEmpty()) {
        lsofExe = QStringLiteral("lsof"); // maybe QProcess has more luck
    }

    QProcess proc;
    proc.setProcessChannelMode(QProcess::SeparateChannels);
    proc.setReadChannel(QProcess::StandardOutput);
    proc.start(lsofExe, QStringList() << QStringLiteral("-Fn") << QStringLiteral("-n") << QStringLiteral("-p") << QString::number(pid));
    proc.waitForFinished();

    forever {
        const QByteArray line = proc.readLine();
        if (line.isEmpty())
            break;

        if (containsQtCore(line))
            return QString::fromLocal8Bit(line.mid(1).trimmed()); // strip the field identifier
    }

    return QString();
}

static bool checkQtCorePrefix(const QByteArray &line, int index)
{
    Q_ASSERT(index >= 0);
    if (index == 0)
        return true;

    // we either have a "lib" prefix, or some sort of separator
    if (index >= 3 && line.indexOf("lib", index - 3) == index - 3)
        return true;

    if ((line.at(index - 1) >= 'a' && line.at(index - 1) <= 'z') || (line.at(index - 1) >= 'A' && line.at(index - 1) <= 'Z'))
        return false;

    return true;
}

static bool checkQtCoreSuffix(const QByteArray &line, int index)
{
    if (index >= line.size())
        return false;

    Q_ASSERT(line.at(index - 2) == 'Q' && line.at(index - 1) == 't');

    // skip version numbers
    while (index < line.size() && line.at(index) >= '0' && line.at(index) <= '9')
        ++index;

    const auto core = QByteArrayLiteral("Core" QT_LIBINFIX);
    if (line.indexOf(core, index) != index)
        return false;

    // skip Core + QT_LIBINFIX
    index += core.size();

    // deal with the "d" and "_debug" debug suffixes
    if (index < line.size() && line.at(index) == 'd')
        ++index;

    // "Core" must not be followed by another part of the name, so we don't trigger on eg. "QtCoreAddon"
    if (index < line.size() && ((line.at(index) >= 'a' && line.at(index) <= 'z') || (line.at(index) >= 'A' && line.at(index) <= 'Z')))
        return false;

    // must not be followed by .abi3, (pyside)
    if (line.lastIndexOf(".abi3", index) == index) {
        return false;
    }

    // Sometimes when using PySide2 on Windows we end up loading QtCore.pyd, which doesn't
    // have information about the corresponding Qt version among its file details.
    // We can safely skip it since that will in turn load a QtCore dll file with the right
    // information, and that will be picked up by this function.
    if (line.endsWith(".pyd")) {
        return false;
    }

    return true;
}

bool ProbeABIDetector::containsQtCore(const QByteArray &line)
{
    // Unix: libQt[X]Core.so[.X.Y.Z]$
    // Mac: [^/]Qt[X]Core[_debug]$, [lib]Qt[X]Core[_debug].dylib[.X.Y.Z]$
    // Windows Qt[X]Core[d].dll

    for (int index = 0; (index = line.indexOf("Qt", index)) >= 0; ++index) {
        // Path must not be something like "libqtqmlcoreplugin" which is not what we're looking for
        if (line.contains(QByteArrayLiteral("qml")))
            return false;

        if (!checkQtCorePrefix(line, index))
            continue;

        if (checkQtCoreSuffix(line, index + 2))
            return true;
    }

    return false;
}
