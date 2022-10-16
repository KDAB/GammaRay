/*
  probefinder.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <config-gammaray.h>
#include "probefinder.h"
#include "probeabi.h"

#include <common/paths.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include <QString>

#include <algorithm>

namespace GammaRay {
namespace ProbeFinder {

static QString findProbeInternalNormalLayout(const ProbeABI &abi, const QString &rootPath)
{
#ifndef GAMMARAY_INSTALL_QT_LAYOUT
    const QString path = rootPath + QLatin1String("/" GAMMARAY_PLUGIN_INSTALL_DIR "/" GAMMARAY_PLUGIN_VERSION "/") + abi.id();
    const QString wildcard = QLatin1String(GAMMARAY_PROBE_BASENAME "*") + Paths::libraryExtension();
    const auto fi = QDir(path).entryInfoList(QStringList(wildcard)).value(0);
    if (fi.isFile() && fi.isReadable())
        return fi.canonicalFilePath();
#else
    Q_UNUSED(abi);
    Q_UNUSED(rootPath);
#endif
    return QString();
}

static QString findProbeInternalQtLayout(const ProbeABI &abi, const QString &rootPath)
{
#ifndef GAMMARAY_INSTALL_QT_LAYOUT
#ifdef Q_OS_WIN
    const QString path = rootPath + QLatin1String("/bin/" GAMMARAY_PROBE_BASENAME "-") + abi.id() + Paths::libraryExtension();
#else
    const QString path = rootPath + QLatin1String("/lib/libgammaray_probe-") + abi.id() + Paths::libraryExtension();
#endif
#else
    const QString path = rootPath + QLatin1String("/" GAMMARAY_PROBE_INSTALL_DIR "/" GAMMARAY_PROBE_BASENAME "-") + abi.id() + Paths::libraryExtension();
#endif

    const auto fi = QFileInfo(path);
    if (fi.isFile() && fi.isReadable())
        return fi.canonicalFilePath();
    return QString();
}

static QString findProbeInternal(const ProbeABI &probeAbi, const QString &rootPath)
{
#ifndef GAMMARAY_INSTALL_QT_LAYOUT
    const auto probe = findProbeInternalNormalLayout(probeAbi, rootPath);
    if (!probe.isEmpty())
        return probe;
    return findProbeInternalQtLayout(probeAbi, rootPath);
#else
    const auto probe = findProbeInternalQtLayout(probeAbi, rootPath);
    if (!probe.isEmpty())
        return probe;
    return findProbeInternalNormalLayout(probeAbi, rootPath);
#endif
}

QString findProbe(const ProbeABI &probeAbi, const QStringList &searchRoots)
{
    for (const auto &searchRoot : searchRoots) {
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
        << "This is likely a setup problem, due to an incomplete or partially moved installation.";
    return QString();
}

ProbeABI findBestMatchingABI(const ProbeABI &targetABI)
{
    return findBestMatchingABI(targetABI, listProbeABIs());
}

ProbeABI findBestMatchingABI(const ProbeABI &targetABI, const QVector<ProbeABI> &availableABIs)
{
    QVector<ProbeABI> compatABIs;
    for (const ProbeABI &abi : availableABIs) {
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
