/*
  probefinder.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QString>
#include <QStringBuilder>

namespace GammaRay {

namespace ProbeFinder {

QString findProbe(const QString &baseName, const QString &probeAbi)
{
  const QString probePath =
    QCoreApplication::applicationDirPath() %
    QDir::separator() %
    QLatin1Literal(GAMMARAY_RELATIVE_PROBE_PATH) %
    QDir::separator() %
    QLatin1Literal(GAMMARAY_PLUGIN_VERSION) %
    QDir::separator() %
    probeAbi %
    QDir::separator() %
    baseName %
    fileExtension();

  if (!QFile::exists(probePath)) {
    qWarning() << "Cannot locate probe" << probePath;
    qWarning() << "This is likely a setup problem, due to an incomplete or partially moved installation.";
    return QString();
  }

  return probePath;
}

QStringList listProbeABIs()
{
  const QString path =
    QCoreApplication::applicationDirPath() %
    QDir::separator() %
    QLatin1Literal(GAMMARAY_RELATIVE_PROBE_PATH) %
    QDir::separator() %
    QLatin1Literal(GAMMARAY_PLUGIN_VERSION);
  const QDir dir(path);
  return dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
}

QString fileExtension()
{
#ifdef Q_OS_WIN
  return QLatin1String(".dll");
#elif defined(Q_OS_MAC)
  return QLatin1String(".dylib");
#else
  return QLatin1String(".so");
#endif
}

}

}
