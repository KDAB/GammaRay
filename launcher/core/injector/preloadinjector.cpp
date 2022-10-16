/*
  preloadinjector.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "preloadinjector.h"

#include <libraryutil.h>
#include <probeabidetector.h>

#include <QProcess>
#include <QDebug>
#include <QStandardPaths>

#include <cstdlib>

using namespace GammaRay;

PreloadInjector::PreloadInjector() = default;

QString PreloadInjector::name() const
{
    return QStringLiteral("preload");
}

bool PreloadInjector::launch(const QStringList &programAndArgs, const QString &probeDll,
                             const QString &probeFunc, const QProcessEnvironment &_env)
{
    Q_UNUSED(probeFunc);

    QProcessEnvironment env(_env);
#ifdef Q_OS_MAC
    env.insert(QStringLiteral("DYLD_INSERT_LIBRARIES"), probeDll);
    env.insert(QStringLiteral("GAMMARAY_UNSET_DYLD"), QStringLiteral("1"));

    // Make sure Qt do load it's correct libs/plugins.
    if (probeDll.contains(QStringLiteral("_debug"), Qt::CaseInsensitive))
        env.insert(QStringLiteral("DYLD_IMAGE_SUFFIX"), QStringLiteral("_debug"));

#else

    auto exePath = programAndArgs.first();
    exePath = QStandardPaths::findExecutable(exePath);

    // ASAN requires to be loaded first, so check if the target uses that
    // and if so inject it before GammaRay
    QStringList ldPreload;
    foreach (const auto &lib, LibraryUtil::dependencies(exePath)) {
        if (lib.contains("libasan.so") || lib.contains("libclang_rt.asan")) {
            ldPreload.push_back(QString::fromLocal8Bit(lib));
            break;
        }
    }
    ldPreload.push_back(probeDll);
    env.insert(QStringLiteral("LD_PRELOAD"), ldPreload.join(QLatin1String(":")));
    env.insert(QStringLiteral("GAMMARAY_UNSET_PRELOAD"), QStringLiteral("1"));

#endif

    return launchProcess(programAndArgs, env);
}
