/*
  modelinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "modelinspector.h"

#include "modelinspectorwidget.h"
#include "modelmodel.h"
#include "modeltester.h"

#include "include/probeinterface.h"

using namespace GammaRay;

ModelInspector::ModelInspector(QObject *parent) :
  QObject(parent),
  m_modelModel(0),
  m_modelTester(0)
{
}

QString ModelInspector::id() const
{
  return metaObject()->className();
}

QString ModelInspector::name() const
{
 return tr("Models");
}

QStringList ModelInspector::supportedTypes() const
{
  return QStringList(QAbstractItemModel::staticMetaObject.className());
}

void ModelInspector::init(ProbeInterface *probe)
{
  m_modelModel = new ModelModel(this);
  connect(probe->probe(), SIGNAL(objectCreated(QObject*)),
          m_modelModel, SLOT(objectAdded(QObject*)));
  connect(probe->probe(), SIGNAL(objectDestroyed(QObject*)),
          m_modelModel, SLOT(objectRemoved(QObject*)));

  m_modelTester = new ModelTester(this);
  connect(probe->probe(), SIGNAL(objectCreated(QObject*)),
          m_modelTester, SLOT(objectAdded(QObject*)));
}

QWidget *ModelInspector::createWidget(ProbeInterface *probe, QWidget *parentWidget)
{
  return new ModelInspectorWidget(this, probe, parentWidget);
}

ModelModel *ModelInspector::modelModel() const
{
  return m_modelModel;
}

#include "modelinspector.moc"
