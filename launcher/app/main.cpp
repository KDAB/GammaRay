/*
  main.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <launcher/ui/launcherwindow.h>
#include <launcher/core/launchoptions.h>
#include <launcher/core/launcherfinder.h>

#include <common/paths.h>
#include <common/translator.h>

#include <QApplication>

#include <iostream>

using namespace GammaRay;

int main(int argc, char **argv)
{
    std::cout << "Startup: gammaray-launcher" << std::endl;

    QCoreApplication::setOrganizationName(QStringLiteral("KDAB"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kdab.com"));
    QCoreApplication::setApplicationName(QStringLiteral("GammaRay"));
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    QGuiApplication::setDesktopFileName(QStringLiteral("GammaRay.desktop"));
#endif

    QApplication app(argc, argv);
    Paths::setRelativeRootPath(GAMMARAY_INVERSE_LIBEXEC_DIR);
    Translator::loadStandAloneTranslations();

    LauncherWindow launcher;
    launcher.show();
    const int result = app.exec();

    if (launcher.result() == QDialog::Accepted) {
        const LaunchOptions opts = launcher.launchOptions();
        if (opts.isValid())
            opts.execute(LauncherFinder::findLauncher(LauncherFinder::Injector));
    }

    return result;
}
