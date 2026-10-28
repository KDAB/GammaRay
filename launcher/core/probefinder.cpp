/*
  probefinder.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>
#include "probefinder.h"
#include "probeabi.h"
#include "launcherlog.h"

#include <common/paths.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include <QString>

#include <algorithm>
#include <functional>
#include <iterator>

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

QString mismatchReason(const ProbeABI &targetABI)
{
    return mismatchReason(targetABI, listProbeABIs());
}

static QString tr(const char *text)
{
    return QCoreApplication::translate("GammaRay::ProbeFinder", text);
}

static QString qtVersion(const ProbeABI &abi)
{
    return QStringLiteral("%1.%2").arg(abi.majorQtVersion()).arg(abi.minorQtVersion());
}

// the distinct values of the given probes, for the messages below
template<typename Func>
static QString valuesOf(const QVector<ProbeABI> &abis, Func value)
{
    QStringList values;
    for (const auto &abi : abis) {
        const QString v = value(abi);
        if (!values.contains(v))
            values.push_back(v);
    }
    return values.join(QStringLiteral(", "));
}

template<typename Func>
static QVector<ProbeABI> filtered(const QVector<ProbeABI> &abis, Func accept)
{
    QVector<ProbeABI> result;
    std::copy_if(abis.cbegin(), abis.cend(), std::back_inserter(result), accept);
    return result;
}

QString mismatchReason(const ProbeABI &targetABI, const QVector<ProbeABI> &availableABIs)
{
    if (!targetABI.isValid())
        return tr("The Qt ABI of the target is unknown.");
    if (availableABIs.isEmpty())
        return tr("No probes found in %1.").arg(Paths::probePath(QString()));
    if (findBestMatchingABI(targetABI, availableABIs).isValid())
        return QString();

    // narrow the probes down in the order of ProbeABI::isCompatible(), and report the first
    // property none of the remaining ones matches
    auto candidates = filtered(availableABIs, [&](const ProbeABI &abi) {
        return abi.majorQtVersion() == targetABI.majorQtVersion();
    });
    if (candidates.isEmpty()) {
        return tr("The target uses Qt %1, the probes are for Qt %2.")
            .arg(qtVersion(targetABI), valuesOf(availableABIs, qtVersion));
    }

    auto remaining = filtered(candidates, [&](const ProbeABI &abi) {
        return abi.architecture() == targetABI.architecture();
    });
    if (remaining.isEmpty()) {
        return tr("The target is built for %1, the probes for %2.")
            .arg(targetABI.architecture(), valuesOf(candidates, std::mem_fn(&ProbeABI::architecture)));
    }
    candidates = remaining;

#ifdef Q_OS_WIN
    const auto compiler = [](const ProbeABI &abi) {
        return abi.isVersionRelevant() ? abi.compiler() + QLatin1Char(' ') + abi.compilerVersion() : abi.compiler();
    };
    remaining = filtered(candidates, [&](const ProbeABI &abi) {
        return compiler(abi) == compiler(targetABI);
    });
    if (remaining.isEmpty()) {
        return tr("The Qt of the target is built with %1, the probes with %2.")
            .arg(compiler(targetABI), valuesOf(candidates, compiler));
    }
    candidates = remaining;
#endif

    if (ProbeABI::isDebugRelevant()) {
        remaining = filtered(candidates, [&](const ProbeABI &abi) {
            return abi.isDebug() == targetABI.isDebug();
        });
        if (remaining.isEmpty()) {
            return targetABI.isDebug()
                ? tr("The target uses a debug build of Qt, the probes are release builds.")
                : tr("The target uses a release build of Qt, the probes are debug builds. "
                     "GammaRay needs to be built in release mode for it.");
        }
        candidates = remaining;
    }

    // probes for a newer Qt than the target's are left
    const auto oldest = *std::min_element(candidates.cbegin(), candidates.cend());
    return tr("The target uses Qt %1, the probes need Qt %2 or later.")
        .arg(qtVersion(targetABI), qtVersion(oldest));
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
    qCInfo(launcherLog) << "Probes in" << dir.path() << ":" << valuesOf(abis, std::mem_fn(&ProbeABI::id));
    return abis;
}
}
}
