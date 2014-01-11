/*
  probecreator.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "probecreator.h"

#include <common/endpoint.h>
#include <core/probe.h>

#include <QCoreApplication>
#include <QMetaObject>
#include <QVector>
#include <QThread>

#include <iostream>

#define IF_DEBUG(x)

using namespace GammaRay;
using namespace std;

ProbeCreator::ProbeCreator(Type type)
  : m_type(type)
{
  //push object into the main thread, as windows creates a
  //different thread where this runs in
  moveToThread(QCoreApplication::instance()->thread());
  // delay to foreground thread
  QMetaObject::invokeMethod(this, "createProbe", Qt::QueuedConnection);

  // don't propagate the probe to child processes
  if (qgetenv("GAMMARAY_UNSET_PRELOAD") == "1") {
    qputenv("LD_PRELOAD", "");
  }
  if (qgetenv("GAMMARAY_UNSET_DYLD") == "1") {
    qputenv("DYLD_INSERT_LIBRARIES", "");
    qputenv("DYLD_FORCE_FLAT_NAMESPACE", "");
  }

  // HACK the webinspector plugin does this as well, but if the web view is created
  // too early the env var from there isn't going to reach the web process
  qputenv("QTWEBKIT_INSPECTOR_SERVER", "0.0.0.0:" + QByteArray::number(Endpoint::defaultPort() + 1));
}

void ProbeCreator::createProbe()
{
  // make sure we are in the ui thread
  Q_ASSERT(QThread::currentThread() == qApp->thread());

  if (!qApp || Probe::isInitialized()) {
    // never create it twice
    deleteLater();
    return;
  }

  Probe::createProbe(m_type == GammaRay::ProbeCreator::CreateAndFindExisting);
  Q_ASSERT(Probe::isInitialized());

  deleteLater();
}

