/*
  probecreator.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "probe.h"

#include <QApplication>
#include <QMetaObject>
#include <QVector>
#include <QThread>

#include <iostream>

#define IF_DEBUG(x)

using namespace GammaRay;
using namespace std;

Q_GLOBAL_STATIC(QVector<QObject*>, s_addedBeforeProbeInsertion)

ProbeCreator::ProbeCreator(Type type)
  : m_type(type)
{
  //push object into the main thread, as windows creates a
  //different thread where this runs in
  moveToThread(QApplication::instance()->thread());
  // delay to foreground thread
  QMetaObject::invokeMethod(this, "createProbe", Qt::QueuedConnection);
}

void ProbeCreator::trackObject(QObject* object)
{
  QWriteLocker lock(Probe::objectLock());

  // need to pay attention if the Probe asked to filter certain objects
  // e.g. during the construction of the Probe itself
  if (Probe::filteredThread() == object->thread())
    return;

  s_addedBeforeProbeInsertion()->push_back(object);
}

void ProbeCreator::untrackObject(QObject* obj)
{
  QWriteLocker lock(Probe::objectLock());

  if (!s_addedBeforeProbeInsertion())
    return;

  for (QVector<QObject*>::iterator it = s_addedBeforeProbeInsertion()->begin();
      it != s_addedBeforeProbeInsertion()->end();) {
    if (*it == obj) {
      it = s_addedBeforeProbeInsertion()->erase(it);
    } else {
      ++it;
    }
  }
}

void ProbeCreator::createProbe()
{
  QWriteLocker lock(Probe::objectLock());
  // make sure we are in the ui thread
  Q_ASSERT(QThread::currentThread() == qApp->thread());

  if (!qApp || Probe::isInitialized()) {
    // never create it twice
    return;
  }

  const bool success = Probe::createProbe();
  Q_ASSERT(success);

  Q_ASSERT(Probe::instance());
  QMetaObject::invokeMethod(Probe::instance(), "delayedInit", Qt::QueuedConnection);

  // add objects to the probe that were tracked before its creation
  foreach (QObject *obj, *(s_addedBeforeProbeInsertion())) {
    Probe::objectAdded(obj);
  }
  s_addedBeforeProbeInsertion()->clear();

  if (m_type == CreateAndFindExisting) {
    Probe::findExistingObjects();
  }

  deleteLater();
}

#include "probecreator.moc"
