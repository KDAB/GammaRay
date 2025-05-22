/*
  main.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    QGuiApplication::setDesktopFileName(QStringLiteral("GammaRay"));

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
