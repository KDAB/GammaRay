/*
  benchsuite.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "benchsuite.h"
#include "core/connectionmodel.h"
#include "core/probe.h"
#include "core/util.h"

#include <QtTestGui>

#include <QLabel>
#include <QTreeView>

QTEST_MAIN(GammaRay::BenchSuite)

using namespace GammaRay;

void BenchSuite::iconForObject()
{
  QWidget widget;
  QLabel label;
  QTreeView treeView;
  QBENCHMARK {
    Util::iconForObject(this);
    Util::iconForObject(&widget);
    Util::iconForObject(&label);
    Util::iconForObject(&treeView);
  }
}

void BenchSuite::connectionModel_connectionAdded()
{
  Probe::createProbe(false);

  ConnectionModel model;
  static const int NUM_OBJECTS = 10000;
  QVector<QObject*> objects;
  objects.reserve(NUM_OBJECTS + 1);
  // fill it
  objects << new QObject;
  for (int i = 1; i <= NUM_OBJECTS; ++i) {
    QObject *obj = new QObject;
    objects << obj;
    Probe::objectAdded(obj);
  }

  QBENCHMARK {
    for (int i = 1; i <= NUM_OBJECTS; ++i) {
      model.connectionAdded(objects.at(i), SIGNAL(destroyed()),
                            objects.at(i-1), SLOT(deleteLater()), Qt::AutoConnection);
      model.connectionAdded(objects.at(i-1), SIGNAL(destroyed()),
                            objects.at(i), SLOT(deleteLater()), Qt::AutoConnection);
    }
  }

  qDeleteAll(objects);
  delete Probe::instance();
}

void BenchSuite::connectionModel_connectionRemoved()
{
  Probe::createProbe(false);

  ConnectionModel model;
  static const int NUM_OBJECTS = 1000;
  QVector<QObject*> objects;
  objects.reserve(NUM_OBJECTS + 1);
  // fill it
  objects << new QObject;
  for (int i = 1; i <= NUM_OBJECTS; ++i) {
    QObject *obj = new QObject;
    objects << obj;
    Probe::objectAdded(obj);
    model.connectionAdded(obj, SIGNAL(destroyed()),
                          objects.at(i-1), SLOT(deleteLater()), Qt::AutoConnection);

    model.connectionAdded(objects.at(i-1), SIGNAL(destroyed()),
                          obj, SLOT(deleteLater()), Qt::AutoConnection);

    model.connectionAdded(obj, SIGNAL(destroyed()),
                          objects.at(i-1), SLOT(deleteLater()), Qt::AutoConnection);

    model.connectionAdded(objects.at(i-1), SIGNAL(destroyed()),
                          obj, SLOT(deleteLater()), Qt::AutoConnection);

    model.connectionAdded(obj, SIGNAL(invalid()),
                          objects.at(i-1), SLOT(deleteLater()), Qt::AutoConnection);

    model.connectionAdded(objects.at(i-1), SIGNAL(destroyed()),
                          obj, SLOT(invalid()), Qt::AutoConnection);

//krazy:cond=normalized,style
    // non-normalized
    model.connectionAdded(obj, SIGNAL( destroyed(  ) ),
                          objects.at(i-1), SLOT(deleteLater()), Qt::AutoConnection);

    model.connectionAdded(objects.at(i-1), SIGNAL(destroyed()),
                          obj, SLOT( deleteLater(  ) ), Qt::AutoConnection);
//krazy:endcond=normalized,style
  }

  QBENCHMARK_ONCE {
    for (int i = 1; i <= NUM_OBJECTS; ++i) {
      model.connectionRemoved(objects.at(i), SIGNAL(destroyed()),
                              objects.at(i-1), SLOT(deleteLater()));

      model.connectionRemoved(objects.at(i-1), SIGNAL(destroyed()),
                              objects.at(i), SLOT(deleteLater()));

      model.connectionRemoved(objects.at(i), SIGNAL(destroyed()),
                              objects.at(i-1), SLOT(invalid()));

      model.connectionRemoved(objects.at(i-1), SIGNAL(invalid()),
                              objects.at(i), SLOT(deleteLater()));
    }
  }

  qDeleteAll(objects);
  delete Probe::instance();
}

void BenchSuite::probe_objectAdded()
{
  Probe::createProbe(false);

  static const int NUM_OBJECTS = 10000;
  QVector<QObject*> objects;
  objects.reserve(NUM_OBJECTS + 1);
  // fill it
  for (int i = 0; i < NUM_OBJECTS; ++i) {
    QObject *obj = new QObject;
    objects << obj;
  }

  QVector<QObject*>::const_iterator it = objects.constBegin();
  QVector<QObject*>::const_iterator end = objects.constEnd();
  QBENCHMARK_ONCE {
    while (it != end) {
      Probe::objectAdded(*it);
      ++it;
    }
  }

  qDeleteAll(objects);
  delete Probe::instance();
}

