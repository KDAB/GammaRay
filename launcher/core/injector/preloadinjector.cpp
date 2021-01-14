/*
  preloadinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
