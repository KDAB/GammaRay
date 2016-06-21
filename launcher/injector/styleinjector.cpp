/*
  styleinjector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "styleinjector.h"

#include <common/paths.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#ifdef HAVE_QT_WIDGETS
#include <QStyleFactory>
#endif

#include <cstdlib>

using namespace GammaRay;

StyleInjector::StyleInjector()
    : ProcessInjector()
{
}

QString StyleInjector::name() const
{
    return QStringLiteral("style");
}

bool StyleInjector::launch(const QStringList &programAndArgs, const QString &probeDll,
                           const QString &probeFunc, const QProcessEnvironment &e)
{
    auto env = e.isEmpty() ? QProcessEnvironment::systemEnvironment() : e;
    env.insert(QStringLiteral("GAMMARAY_STYLEINJECTOR_PROBEDLL"), probeDll);
    env.insert(QStringLiteral("GAMMARAY_STYLEINJECTOR_PROBEFUNC"), probeFunc);

    QString qtPluginPath = env.value(QStringLiteral("QT_PLUGIN_PATH"));
    if (!qtPluginPath.isEmpty())
        qtPluginPath.append(":");
    qtPluginPath.append(Paths::currentPluginsPath());
    env.insert(QStringLiteral("QT_PLUGIN_PATH"), qtPluginPath);

    QStringList args = programAndArgs;
    args << QStringLiteral("-style") << QStringLiteral("gammaray-injector");

    return launchProcess(args, env);
}

bool StyleInjector::selfTest()
{
#ifdef HAVE_QT_WIDGETS
    QCoreApplication::addLibraryPath(Paths::currentPluginsPath());
    if (!QStyleFactory::keys().contains(QStringLiteral("gammaray-injector"))) {
        mErrorString = tr("Injector style plugin is not found in the Qt style "
                          "plug-in search path or cannot be loaded");
        return false;
    }

    return true;
#else
    mErrorString = tr(
        "GammaRay was compiled without QtWidget support, style injector is not available.");
    return false;
#endif
}
