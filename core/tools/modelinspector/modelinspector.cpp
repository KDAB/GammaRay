/*
  modelinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <network/objectbroker.h>

#include <QAbstractItemView>
#include <QComboBox>

using namespace GammaRay;

ModelInspector::ModelInspector(ProbeInterface* probe, QObject *parent) :
  QObject(parent),
  m_modelModel(0),
  m_modelTester(0)
{
  m_modelModel = new ModelModel(this);
  connect(probe->probe(), SIGNAL(objectCreated(QObject*)),
          m_modelModel, SLOT(objectAdded(QObject*)));
  connect(probe->probe(), SIGNAL(objectDestroyed(QObject*)),
          m_modelModel, SLOT(objectRemoved(QObject*)));
  probe->registerModel("com.kdab.GammaRay.ModelModel", m_modelModel);

  m_modelSelectionModel = ObjectBroker::selectionModel(m_modelModel);
  connect(probe->probe(), SIGNAL(widgetSelected(QWidget*,QPoint)), SLOT(widgetSelected(QWidget*)) );

  m_modelTester = new ModelTester(this);
  connect(probe->probe(), SIGNAL(objectCreated(QObject*)),
          m_modelTester, SLOT(objectAdded(QObject*)));
}

QString ModelInspectorFactory::name() const
{
 return tr("Models");
}

void ModelInspector::widgetSelected(QWidget* widget)
{
  QAbstractItemModel *selectedModel = 0;

  QAbstractItemView *view = Util::findParentOfType<QAbstractItemView>(widget);
  if (view)
    selectedModel = view->model();

  QComboBox *box = Util::findParentOfType<QComboBox>(widget);
  if (!selectedModel && box)
    selectedModel = box->model();

  if (selectedModel) {
    const QModelIndexList indexList =
      m_modelModel->match(m_modelModel->index(0, 0),
                   ObjectModel::ObjectRole,
                   QVariant::fromValue<QObject*>(selectedModel), 1,
                   Qt::MatchExactly | Qt::MatchRecursive);
    if (indexList.isEmpty()) {
      return;
    }

    const QModelIndex index = indexList.first();
    m_modelSelectionModel->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
  }
}

#include "modelinspector.moc"
