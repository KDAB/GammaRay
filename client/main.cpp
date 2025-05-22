/*
  main.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2013 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include <config-gammaray.h>
#include <config-gammaray-version.h>

#include "client.h"
#include "clientconnectionmanager.h"

#include <common/objectbroker.h>
#include <common/paths.h>
#include <common/translator.h>

#include <QApplication>
#include <QStringList>

using namespace GammaRay;

int main(int argc, char **argv)
{
    QCoreApplication::setOrganizationName(QStringLiteral("KDAB"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kdab.com"));
    QCoreApplication::setApplicationName(QStringLiteral("GammaRay"));
    QCoreApplication::setApplicationVersion(QStringLiteral(GAMMARAY_COMPACT_VERSION_STRING));
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts); // for QWebEngine
    QGuiApplication::setDesktopFileName(QStringLiteral("GammaRay"));

    QApplication app(argc, argv);
    Paths::setRelativeRootPath(GAMMARAY_INVERSE_LIBEXEC_DIR);
    Translator::loadStandAloneTranslations();
    ClientConnectionManager::init();

    QUrl serverUrl;
    if (app.arguments().size() == 2) {
        serverUrl = QUrl::fromUserInput(app.arguments().at(1));
    } else {
        serverUrl.setScheme(QStringLiteral("tcp"));
        serverUrl.setHost(QStringLiteral(GAMMARAY_DEFAULT_LOCAL_ADDRESS));
        serverUrl.setPort(Client::defaultPort());
    }

    ClientConnectionManager conMan;
    QObject::connect(&conMan, &ClientConnectionManager::ready, &conMan, &ClientConnectionManager::createMainWindow);
    QObject::connect(&conMan, &ClientConnectionManager::disconnected, QApplication::instance(), &QCoreApplication::quit);
    QObject::connect(&conMan, &ClientConnectionManager::persistentConnectionError, &conMan,
                     &ClientConnectionManager::handlePersistentConnectionError);
    conMan.connectToHost(serverUrl);
    return app.exec();
}
