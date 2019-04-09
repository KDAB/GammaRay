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
    QCoreApplication::setOrganizationName("KDAB");
    QCoreApplication::setOrganizationDomain("kdab.com");
    QCoreApplication::setApplicationName("GammaRay");
    QCoreApplication::setApplicationVersion(GAMMARAY_COMPACT_VERSION_STRING);
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QCoreApplication::setAttribute(Qt::AA_ShareOpenGLContexts); // for QWebEngine
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    QGuiApplication::setDesktopFileName(QStringLiteral("GammaRay.desktop"));
#endif

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
