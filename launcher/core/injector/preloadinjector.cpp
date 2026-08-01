/*
  preloadinjector.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "preloadinjector.h"

#include <libraryutil.h>
#include <probeabidetector.h>

#include <QProcess>
#include <QDebug>
#include <QStandardPaths>

#include <cstdlib>

using namespace GammaRay;

PreloadInjector::PreloadInjector(const QString &probeDllOverride)
    : ProcessInjector()
    , m_probeDllOverride(probeDllOverride)
{
}

QString PreloadInjector::name() const
{
    return QStringLiteral("preload");
}

bool PreloadInjector::launch(const QStringList &programAndArgs, const QString &probeDll,
                             const QString &probeFunc, const QProcessEnvironment &_env)
{
    Q_UNUSED(probeFunc);

    const QString actualProbeDll = m_probeDllOverride.isEmpty() ? probeDll : m_probeDllOverride;

    QProcessEnvironment env(_env);
#ifdef Q_OS_MAC
    env.insert(QStringLiteral("DYLD_INSERT_LIBRARIES"), actualProbeDll);
    env.insert(QStringLiteral("GAMMARAY_UNSET_DYLD"), QStringLiteral("1"));

    // Make sure Qt do load it's correct libs/plugins.
    if (actualProbeDll.contains(QStringLiteral("_debug"), Qt::CaseInsensitive))
        env.insert(QStringLiteral("DYLD_IMAGE_SUFFIX"), QStringLiteral("_debug"));

#else

    auto exePath = programAndArgs.first();
    exePath = QStandardPaths::findExecutable(exePath);

    // ASAN requires to be loaded first, so check if the target uses that
    // and if so inject it before GammaRay
    QStringList ldPreload;
    for (const auto &lib: LibraryUtil::dependencies(exePath)) {
        if (lib.contains("libasan.so") || lib.contains("libclang_rt.asan")) {
            ldPreload.push_back(QString::fromLocal8Bit(lib));
            break;
        }
    }
    ldPreload.push_back(actualProbeDll);
    env.insert(QStringLiteral("LD_PRELOAD"), ldPreload.join(QLatin1String(":")));
    env.insert(QStringLiteral("GAMMARAY_UNSET_PRELOAD"), QStringLiteral("1"));

#endif

    return launchProcess(programAndArgs, env);
}
