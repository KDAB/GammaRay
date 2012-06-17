/*
  objectinspector.cpp

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

#include "objectinspector.h"
#include "ui_objectinspector.h"

#include "include/objectmodel.h"
#include "include/probeinterface.h"

#include <kde/krecursivefilterproxymodel.h>

#include <QLineEdit>

using namespace GammaRay;

ObjectInspector::ObjectInspector(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent),
    ui(new Ui::ObjectInspector)
{
  ui->setupUi(this);

  QSortFilterProxyModel *objectFilter = new KRecursiveFilterProxyModel(this);
  objectFilter->setSourceModel(probe->objectTreeModel());
  objectFilter->setDynamicSortFilter(true);
  ui->objectTreeView->setModel(objectFilter);
  ui->objectTreeView->header()->setResizeMode(0, QHeaderView::Stretch);
  ui->objectTreeView->header()->setResizeMode(1, QHeaderView::Interactive);
  ui->objectSearchLine->setProxy(objectFilter);
  connect(ui->objectTreeView->selectionModel(),
          SIGNAL(currentChanged(QModelIndex,QModelIndex)),
          SLOT(objectSelected(QModelIndex)));

  if (qgetenv("GAMMARAY_TEST_FILTER") == "1") {
    QMetaObject::invokeMethod(ui->objectSearchLine->lineEdit(), "setText",
                              Qt::QueuedConnection,
                              Q_ARG(QString, QLatin1String("Object")));
  }

  connect(probe->probe(), SIGNAL(widgetSelected(QWidget*,QPoint)), SLOT(widgetSelected(QWidget*)));
}

void ObjectInspector::objectSelected(const QModelIndex &index)
{
  if (index.isValid()) {
    QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject*>();
    ui->objectPropertyWidget->setObject(obj);
  } else {
    ui->objectPropertyWidget->setObject(0);
  }
}

void ObjectInspector::widgetSelected(QWidget *widget)
{
  QAbstractItemModel *model = ui->objectTreeView->model();
  const QModelIndexList indexList =
  model->match(model->index(0, 0),
               ObjectModel::ObjectRole,
               QVariant::fromValue<QObject*>(widget), 1,
               Qt::MatchExactly | Qt::MatchRecursive);
  if (indexList.isEmpty()) {
    return;
  }

  const QModelIndex index = indexList.first();
  ui->objectTreeView->selectionModel()->select(
    index,
    QItemSelectionModel::Select | QItemSelectionModel::Clear |
    QItemSelectionModel::Rows | QItemSelectionModel::Current);
  ui->objectTreeView->scrollTo(index);
  objectSelected(index);
}

#include "objectinspector.moc"
