/*
  modelinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "modelinspectorwidget.h"
#include "ui_modelinspectorwidget.h"

#include "modelmodel.h"
#include "modelcellmodel.h"
#include "modelinspector.h"
#include "objectmodel.h"

#include <kde/krecursivefilterproxymodel.h>
#include <objectmodel.h>
#include <probeinterface.h>

using namespace GammaRay;

ModelInspectorWidget::ModelInspectorWidget(ModelInspector *modelInspector,
                                           ProbeInterface *probe,
                                           QWidget *parent)
  : QWidget(parent),
    ui(new Ui::ModelInspectorWidget)
{
  Q_UNUSED(probe);
  ui->setupUi(this);

  KRecursiveFilterProxyModel *modelFilterProxy = new KRecursiveFilterProxyModel(this);
  modelFilterProxy->setSourceModel(modelInspector->modelModel());
  ui->modelView->setModel(modelFilterProxy);
  ui->modelSearchLine->setProxy(modelFilterProxy);
  connect(ui->modelView->selectionModel(),
          SIGNAL(currentChanged(QModelIndex,QModelIndex)),
          SLOT(modelSelected(QModelIndex)));
  m_cellModel = new ModelCellModel(this);
  ui->modelCellView->setModel(m_cellModel);

  connect(probe->probe(), SIGNAL(widgetSelected(QWidget*,QPoint)), SLOT(widgetSelected(QWidget*)) );
}

void ModelInspectorWidget::modelSelected(const QModelIndex &index)
{
  if (index.isValid()) {
    QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject*>();
    QAbstractItemModel *model = qobject_cast<QAbstractItemModel*>(obj);
    ui->modelContentView->setModel(model);
    connect(ui->modelContentView->selectionModel(),
            SIGNAL(currentChanged(QModelIndex,QModelIndex)),
            SLOT(modelCellSelected(QModelIndex)));
  } else {
    ui->modelContentView->setModel(0);
  }
  m_cellModel->setModelIndex(QModelIndex());
}

void ModelInspectorWidget::modelCellSelected(const QModelIndex &index)
{
  m_cellModel->setModelIndex(index);
}

void ModelInspectorWidget::widgetSelected(QWidget *widget)
{
  QAbstractItemView *view = Util::findParentOfType<QAbstractItemView>(widget);
  if (view && view->model()) {
    QAbstractItemModel *model = ui->modelView->model();
    const QModelIndexList indexList =
      model->match(model->index(0, 0),
                   ObjectModel::ObjectRole,
                   QVariant::fromValue<QObject*>(view->model()), 1,
                   Qt::MatchExactly | Qt::MatchRecursive);
    if (indexList.isEmpty()) {
      return;
    }

    const QModelIndex index = indexList.first();
    ui->modelView->selectionModel()->select(
      index,
      QItemSelectionModel::Select | QItemSelectionModel::Clear |
      QItemSelectionModel::Rows | QItemSelectionModel::Current);
    ui->modelView->scrollTo(index);
    modelSelected(index);
  }
}

#include "modelinspectorwidget.moc"
