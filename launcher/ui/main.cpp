/*
  main.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include "launcherwindow.h"
#include "launchoptions.h"
#include "launcherfinder.h"

#include <common/paths.h>

#include <QApplication>

using namespace GammaRay;

int main(int argc, char **argv)
{
  QCoreApplication::setOrganizationName("KDAB");
  QCoreApplication::setOrganizationDomain("kdab.com");
  QCoreApplication::setApplicationName("GammaRay");

  QApplication app(argc, argv);
  Paths::setRelativeRootPath(GAMMARAY_INVERSE_LIBEXEC_DIR);

  LauncherWindow launcher;
  launcher.show();
  const int result = app.exec();

  if (launcher.result() == QDialog::Accepted) {
    const LaunchOptions opts = launcher.launchOptions();
    if (opts.isValid()) {
      opts.execute(LauncherFinder::findLauncher(LauncherFinder::Injector));
    }
  }

  return result;
}
