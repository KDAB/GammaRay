/*
  clientlauncher.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_CLIENTLAUNCHER_H
#define GAMMARAY_CLIENTLAUNCHER_H

#include <QProcess>

#include "gammaray_launcher_export.h"

class QUrl;

namespace GammaRay {

/** Launching/monitoring of the GammaRay client for out-of-process use. */
class GAMMARAY_LAUNCHER_EXPORT ClientLauncher
{
public:
  ClientLauncher();
  ~ClientLauncher();

  bool launch(const QUrl &url);
  void terminate();
  void waitForFinished();

  static void launchDetached(const QUrl &url);

private:
  static QString clientPath();
  static QStringList makeArgs(const QUrl &url);

private:
  QProcess m_process;
};
}

#endif // GAMMARAY_CLIENTLAUNCHER_H
