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

#include "config-gammaray.h"
#include "remotemodel.h"
#include "client.h"
#include "selectionmodelclient.h"
#include "clientconnectionmanager.h"
#include "propertycontrollerclient.h"
#include "probecontrollerclient.h"

#include <common/objectbroker.h>
#include <common/streamoperators.h>
#include <common/paths.h>

#include <QApplication>
#include <QStringList>

using namespace GammaRay;

static QAbstractItemModel* modelFactory(const QString &name)
{
  return new RemoteModel(name, qApp);
}

static QItemSelectionModel* selectionModelFactory(QAbstractItemModel* model)
{
  return new SelectionModelClient(model->objectName() + ".selection", model, qApp);
}

static QObject* createPropertyController(const QString &name, QObject *parent)
{
  return new PropertyControllerClient(name, parent);
}

static QObject* createProbeController(const QString &name, QObject *parent)
{
  QObject *o = new ProbeControllerClient(parent);
  ObjectBroker::registerObject(name, o);
  return o;
}

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  Paths::setRelativeRootPath(GAMMARAY_INVERSE_LIBEXEC_DIR);

  StreamOperators::registerOperators();

  QString hostName = QLatin1String("localhost");
  quint16 port = Client::defaultPort();

  if (app.arguments().size() == 2) {
    hostName = app.arguments().at(1);
  } else if (app.arguments().size() == 3) {
    hostName = app.arguments().at(1);
    port = app.arguments().at(2).toUShort();
  }

  ObjectBroker::registerClientObjectFactoryCallback<PropertyControllerInterface*>(createPropertyController);
  ObjectBroker::registerClientObjectFactoryCallback<ProbeControllerInterface*>(createProbeController);
  ObjectBroker::setModelFactoryCallback(modelFactory);
  ObjectBroker::setSelectionModelFactoryCallback(selectionModelFactory);

  ClientConnectionManager conMan;
  conMan.connectToHost(hostName, port);
  return app.exec();
}
