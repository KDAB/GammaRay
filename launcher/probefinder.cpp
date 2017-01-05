/*
  probefinder.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "probefinder.h"
#include "probeabi.h"
#include "probeabidetector.h"

#include <common/paths.h>

#include <qglobal.h>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include <QString>
#include <QStringBuilder>

#include <algorithm>

namespace GammaRay {
namespace ProbeFinder {
static QString findProbeInternal(const ProbeABI &probeAbi, const QString &rootPath)
{
    const QString probePath = Paths::probePath(probeAbi.id(), rootPath)
                              %QDir::separator()
                              %GAMMARAY_PROBE_BASENAME %
#if defined(GAMMARAY_INSTALL_QT_LAYOUT)
                              QChar('-')
                              %probeAbi.id() %
#else
                              QChar('*') %
#endif
                              Paths::libraryExtension();

    const QFileInfo wildcarded(probePath);
    const QFileInfo fi
        = QDir(wildcarded.absolutePath()).entryInfoList(QStringList(wildcarded.fileName())).
          value(0);
    const QString canonicalPath = fi.canonicalFilePath();
    if (!fi.isFile() || !fi.isReadable())
        return QString();
    return canonicalPath;
}

QString findProbe(const QString &baseName, const ProbeABI &probeAbi)
{
    Q_UNUSED(baseName);
    return findProbe(probeAbi);
}

QString findProbe(const ProbeABI &probeAbi, const QStringList &searchRoots)
{
    foreach (const auto &searchRoot, searchRoots) {
        const auto path = findProbeInternal(probeAbi, searchRoot);
        if (!path.isEmpty())
            return path;
    }
    const auto path = findProbeInternal(probeAbi, Paths::rootPath());
    if (!path.isEmpty())
        return path;

    qWarning() << "Cannot locate probe for ABI" << probeAbi.displayString() << " in "
               << searchRoots << Paths::rootPath();
    qWarning()
            <<
            "This is likely a setup problem, due to an incomplete or partially moved installation.";
    return QString();
}

ProbeABI findBestMatchingABI(const ProbeABI &targetABI)
{
    return findBestMatchingABI(targetABI, listProbeABIs());
}

ProbeABI findBestMatchingABI(const ProbeABI &targetABI, const QVector<ProbeABI> &availableABIs)
{
    QVector<ProbeABI> compatABIs;
    foreach (const ProbeABI &abi, availableABIs) {
        if (targetABI.isCompatible(abi))
            compatABIs.push_back(abi);
    }

    if (compatABIs.isEmpty())
        return ProbeABI();

    std::sort(compatABIs.begin(), compatABIs.end());
    return compatABIs.last();
}

QVector<ProbeABI> listProbeABIs()
{
    QVector<ProbeABI> abis;
    const QDir dir(Paths::probePath(QString()));
#if defined(GAMMARAY_INSTALL_QT_LAYOUT)
    const QString filter = QStringLiteral("*gammaray_probe*");
    foreach (const QFileInfo &abiId, dir.entryInfoList(QStringList(filter), QDir::Files)) {
        // OSX has broken QLibrary::isLibrary() - QTBUG-50446
        if (!QLibrary::isLibrary(abiId.fileName())
            && !abiId.fileName().endsWith(Paths::libraryExtension(), Qt::CaseInsensitive))
            continue;
        const ProbeABI abi = ProbeABI::fromString(abiId.baseName().section(QStringLiteral("-"), 1));
        if (abi.isValid())
            abis.push_back(abi);
    }
#else
    foreach (const QString &abiId, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        const ProbeABI abi = ProbeABI::fromString(abiId);
        if (abi.isValid())
            abis.push_back(abi);
    }
#endif
    return abis;
}
}
}
