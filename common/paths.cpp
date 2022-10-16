/*
  paths.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <config-gammaray.h>
#include "paths.h"
#include "selflocator.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLibraryInfo>
#include <QMutex>

namespace GammaRay {

struct PathData
{
    QString rootPath;
    QMutex mutex; // rootPath is set from the probe setting receiver thread
};

Q_GLOBAL_STATIC(PathData, s_pathData)

namespace Paths {

QString rootPath()
{
    QMutexLocker locker(&s_pathData()->mutex);
    if (s_pathData()->rootPath.isEmpty()) {
        QFileInfo fi(SelfLocator::findMe());
        fi.setFile(fi.absolutePath() + QLatin1String("/" GAMMARAY_INVERSE_LIB_DIR));
        if (fi.isDir())
            s_pathData()->rootPath = fi.absoluteFilePath();
    }
    Q_ASSERT(!s_pathData()->rootPath.isEmpty());
    return s_pathData()->rootPath;
}

void setRootPath(const QString &rootPath)
{
    Q_ASSERT(!rootPath.isEmpty());
    Q_ASSERT(QDir(rootPath).exists());
    Q_ASSERT(QDir(rootPath).isAbsolute());

    QMutexLocker locker(&s_pathData()->mutex);
    s_pathData()->rootPath = rootPath;
}

void setRelativeRootPath(const char *relativeRootPath)
{
    Q_ASSERT(relativeRootPath);
    setRootPath(QCoreApplication::applicationDirPath() + QDir::separator()
                + QLatin1String(relativeRootPath));
}

QString probePath(const QString &probeABI, const QString &rootPath)
{
#ifndef GAMMARAY_INSTALL_QT_LAYOUT
    return rootPath + QDir::separator()
        + QLatin1String(GAMMARAY_PLUGIN_INSTALL_DIR) + QDir::separator()
        + QLatin1String(GAMMARAY_PLUGIN_VERSION) + QDir::separator()
        + probeABI;
#else
    Q_UNUSED(probeABI);
    return rootPath + QDir::separator() + QLatin1String(GAMMARAY_PROBE_INSTALL_DIR);
#endif
}

QString binPath()
{
    return rootPath() + QDir::separator() + QLatin1String(GAMMARAY_BIN_INSTALL_DIR);
}

QString libexecPath()
{
    return rootPath() + QDir::separator() + QLatin1String(GAMMARAY_LIBEXEC_INSTALL_DIR);
}

QString currentProbePath()
{
    return probePath(QStringLiteral(GAMMARAY_PROBE_ABI));
}

static void addPluginPath(QStringList &list, const QString &path)
{
    QFileInfo fi(path);
    if (!fi.isDir())
        return;
    list.push_back(fi.canonicalFilePath());
}

QStringList pluginPaths(const QString &probeABI)
{
    QStringList l;
    // TODO based on environment variable for custom plugins

    // based on rootPath()
    addPluginPath(l, rootPath() + QLatin1String("/" GAMMARAY_PLUGIN_INSTALL_DIR "/" GAMMARAY_PLUGIN_VERSION "/") + probeABI);
    addPluginPath(l, rootPath() + QLatin1String("/" GAMMARAY_PLUGIN_INSTALL_DIR));

    // based on Qt plugin search paths
    foreach (const auto &path, QCoreApplication::libraryPaths()) {
        addPluginPath(l, path + QLatin1String("/gammaray/" GAMMARAY_PLUGIN_VERSION "/") + probeABI);
        addPluginPath(l, path + QLatin1String("/gammaray"));

#if defined(Q_OS_ANDROID) && QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
        addPluginPath(l, path);
#endif
    }

    // based on Qt's own install layout and/or qt.conf
    const auto path = QLibraryInfo::location(QLibraryInfo::PluginsPath);
    addPluginPath(l, path + QLatin1String("/gammaray/" GAMMARAY_PLUGIN_VERSION "/") + probeABI);
    addPluginPath(l, path + QLatin1String("/gammaray"));


    return l;
}

QStringList targetPluginPaths(const QString &probeABI)
{
    QStringList l;

    // based on rootPath()
    addPluginPath(l, rootPath() + QLatin1String("/" GAMMARAY_TARGET_PLUGIN_INSTALL_DIR "/" GAMMARAY_PLUGIN_VERSION "/") + probeABI);
    addPluginPath(l, rootPath() + QLatin1String("/" GAMMARAY_TARGET_PLUGIN_INSTALL_DIR));

    // based on Qt plugin search paths
    foreach (const auto &path, QCoreApplication::libraryPaths()) {
        addPluginPath(l, path + QLatin1String("/gammaray/" GAMMARAY_PLUGIN_VERSION "/") + probeABI + QLatin1String("/target"));
        addPluginPath(l, path + QLatin1String("/gammaray-target"));
    }

    // based on Qt's own install layout and/or qt.conf
    const auto path = QLibraryInfo::location(QLibraryInfo::PluginsPath);
    addPluginPath(l, path + QLatin1String("/gammaray/" GAMMARAY_PLUGIN_VERSION "/") + probeABI + QLatin1String("/target"));
    addPluginPath(l, path + QLatin1String("/gammaray-target"));

    return l;
}

QString currentPluginsPath()
{
#ifndef GAMMARAY_INSTALL_QT_LAYOUT
    return probePath(QStringLiteral(GAMMARAY_PROBE_ABI));
#else
    return rootPath() + QDir::separator() + QStringLiteral(GAMMARAY_PLUGIN_INSTALL_DIR);
#endif
}

QString libraryExtension()
{
#ifdef Q_OS_WIN
    return QStringLiteral(".dll");
#elif defined(Q_OS_MAC)
    return QStringLiteral(".dylib");
#elif defined(Q_OS_ANDROID) && QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    return QLatin1Char('_') + QLatin1String(ANDROID_ABI) + QLatin1String(".so");
#else
    return QStringLiteral(".so");
#endif
}

QString pluginExtension()
{
#ifdef Q_OS_MAC
    return QStringLiteral(".so");
#else
    return libraryExtension();
#endif
}

QString documentationPath()
{
    return rootPath() + QLatin1Char('/') + QLatin1String(GAMMARAY_QCH_INSTALL_DIR);
}
}
}
