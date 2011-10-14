/*
  probefinder.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "config-gammaray.h"

#include "probefinder.h"

#include <qglobal.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QString>
#include <QtCore/QStringBuilder>

namespace GammaRay {

namespace ProbeFinder {

QString findProbe(const QString &baseName)
{
#ifndef Q_OS_WIN
  QStringList pldirs;
  pldirs << GAMMARAY_LIB_INSTALL_DIR
         << "/usr/local/lib64" << "/usr/local/lib"
         << "/opt/lib64" << "/opt/lib"
         << "/usr/lib64" << "/usr/lib"
         << GAMMARAY_BUILD_DIR;
  QDir::setSearchPaths("preloads", pldirs);
#ifdef Q_OS_MAC
  QFile plfile(QLatin1Literal("preloads:") % baseName % QLatin1Literal(".dylib"));
#else
  QFile plfile(QLatin1Literal("preloads:") % baseName % QLatin1Literal(".so"));
#endif
  if (plfile.exists()) {
    return plfile.fileName();
  } else {
    qWarning()
      << "Cannot locate" << baseName
      << "in the typical places.\n"
         "Try setting the $LD_PRELOAD environment variable to the fullpath,\n"
         "For example:\n"
         "  export LD_PRELOAD=/opt/lib64/libgammaray_probe.so\n"
         "Continuing nevertheless, some systems can also preload from just the library name...";
    return baseName;
  }

#else
  return
    QCoreApplication::applicationDirPath() %
    QDir::separator() %
    baseName %
    QLatin1Literal(".dll");
#endif

  Q_ASSERT(false);
  return QString();
}

}
}
