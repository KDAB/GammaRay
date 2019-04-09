/*
  probeabidetector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <config-gammaray.h>

#include "probeabidetector.h"

#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>

using namespace GammaRay;

ProbeABIDetector::ProbeABIDetector() = default;

ProbeABI ProbeABIDetector::abiForExecutable(const QString &path) const
{
    return abiForQtCore(qtCoreForExecutable(path));
}

ProbeABI ProbeABIDetector::abiForProcess(qint64 pid) const
{
    return abiForQtCore(qtCoreForProcess(pid));
}

ProbeABI ProbeABIDetector::abiForQtCore(const QString &path) const
{
    QFileInfo fi(path);
    if (!fi.exists())
        return ProbeABI();

    auto it = m_abiForQtCoreCache.constFind(fi.canonicalFilePath());
    if (it != m_abiForQtCoreCache.constEnd())
        return it.value();

    const ProbeABI abi = detectAbiForQtCore(fi.canonicalFilePath());
    m_abiForQtCoreCache.insert(fi.canonicalFilePath(), abi);
    return abi;
}

QString ProbeABIDetector::qtCoreFromLsof(qint64 pid) const
{
    QString lsofExe;
    lsofExe = QStandardPaths::findExecutable(QStringLiteral("lsof"));
    // on OSX it's in sbin, which usually but not always is in PATH...
    if (lsofExe.isEmpty()) {
        lsofExe = QStandardPaths::findExecutable(QStringLiteral("lsof"),
                                                 QStringList() << QStringLiteral(
                                                     "/usr/sbin") << QStringLiteral("/sbin"));
    }
    if (lsofExe.isEmpty()) {
        lsofExe = QStringLiteral("lsof"); // maybe QProcess has more luck
    }

    QProcess proc;
    proc.setProcessChannelMode(QProcess::SeparateChannels);
    proc.setReadChannel(QProcess::StandardOutput);
    proc.start(lsofExe, QStringList() << QStringLiteral("-Fn") << QStringLiteral(
                   "-n") << QStringLiteral("-p") << QString::number(pid));
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

    if ((line.at(index - 1) >= 'a' && line.at(index - 1) <= 'z') ||
        (line.at(index - 1) >= 'A' && line.at(index - 1) <= 'Z'))
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
    if (index < line.size() &&
        ((line.at(index) >= 'a' && line.at(index) <= 'z') ||
         (line.at(index) >= 'A' && line.at(index) <= 'Z')))
        return false;

    return true;
}

bool ProbeABIDetector::containsQtCore(const QByteArray &line)
{
    // Unix: libQt[X]Core.so[.X.Y.Z]$
    // Mac: [^/]Qt[X]Core[_debug]$, [lib]Qt[X]Core[_debug].dylib[.X.Y.Z]$
    // Windows Qt[X]Core[d].dll

    for (int index = 0; (index = line.indexOf("Qt", index)) >= 0; ++index) {
        if (!checkQtCorePrefix(line, index))
            continue;

        if (checkQtCoreSuffix(line, index + 2))
            return true;
    }

    return false;
}
