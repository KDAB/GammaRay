/*
  main.cpp

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

#include <config-gammaray.h>

#include "client.h"
#include "clientconnectionmanager.h"

#include <common/objectbroker.h>
#include <common/paths.h>

#include <QApplication>
#include <QStringList>

using namespace GammaRay;

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  Paths::setRelativeRootPath(GAMMARAY_INVERSE_LIBEXEC_DIR);
  ClientConnectionManager::init();

  QUrl serverUrl;
  if (app.arguments().size() == 2) {
    serverUrl = app.arguments().at(1);
  } else {
    serverUrl.setScheme("tcp");
    serverUrl.setHost("127.0.0.1");
    serverUrl.setPort(Client::defaultPort());
  }

  ClientConnectionManager conMan;
  QObject::connect(&conMan, SIGNAL(ready()), &conMan, SLOT(createMainWindow()));
  QObject::connect(&conMan, SIGNAL(disconnected()), QApplication::instance(), SLOT(quit()));
  QObject::connect(&conMan, SIGNAL(persistentConnectionError(QString)), &conMan, SLOT(handlePersistentConnectionError(QString)));
  conMan.connectToHost(serverUrl);
  return app.exec();
}
