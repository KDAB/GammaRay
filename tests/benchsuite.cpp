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

#include <qtest_gui.h>

#include <QLabel>
#include <QTreeView>

#include <include/util.h>

#include <core/connectionmodel.h>
#include <core/probe.h>

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
  for(int i = 1; i <= NUM_OBJECTS; ++i) {
    QObject *obj = new QObject;
    objects << obj;
    Probe::objectAdded(obj);
  }

  QBENCHMARK {
    for(int i = 1; i <= NUM_OBJECTS; ++i) {
      model.connectionAdded(objects.at(i), SIGNAL(destroyed()), objects.at(i-1), SLOT(deleteLater()), Qt::AutoConnection);
      model.connectionAdded(objects.at(i-1), SIGNAL(destroyed()), objects.at(i), SLOT(deleteLater()), Qt::AutoConnection);
    }
  }

  qDeleteAll(objects);
  delete Probe::instance();
}

#include "benchsuite.moc"
