/*
  paths.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  acuordance with GammaRay Commercial License Agreement provided with the Software.

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
#include "paths.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QString>

namespace GammaRay {
namespace Paths {

static QString s_rootPath;

QString rootPath()
{
  Q_ASSERT(!s_rootPath.isEmpty());
  return s_rootPath;
}

void setRootPath(const QString& rootPath)
{
  Q_ASSERT(!rootPath.isEmpty());
  Q_ASSERT(QDir(rootPath).exists());
  Q_ASSERT(QDir(rootPath).isAbsolute());

  s_rootPath = rootPath;
}

void setRelativeRootPath(const char* relativeRootPath)
{
  Q_ASSERT(relativeRootPath);
  setRootPath(QCoreApplication::applicationDirPath() + QDir::separator() + QLatin1String(relativeRootPath));
}

QString probePath(const QString& probeABI)
{
#ifndef Q_OS_ANDROID
  return rootPath() + QDir::separator()
    + QLatin1String(GAMMARAY_PLUGIN_INSTALL_DIR) + QDir::separator()
    + QLatin1String(GAMMARAY_PLUGIN_VERSION) + QDir::separator()
    + probeABI;
#else
  Q_UNUSED(probeABI);
  return rootPath() + QDir::separator() + QLatin1String(GAMMARAY_PLUGIN_INSTALL_DIR);
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
  return probePath(GAMMARAY_PROBE_ABI);
}

QString libraryExtension()
{
#ifdef Q_OS_WIN
  return QLatin1String(".dll");
#elif defined(Q_OS_MAC)
  return QLatin1String(".dylib");
#else
  return QLatin1String(".so");
#endif
}

QString pluginExtension()
{
#ifdef Q_OS_MAC
  return QLatin1String(".so");
#else
  return libraryExtension();
#endif
}

}

}
