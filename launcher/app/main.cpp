/*
  main.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0) && QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
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
