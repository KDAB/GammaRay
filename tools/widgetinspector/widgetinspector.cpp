/*
  widgetinspector.cpp

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

#include "widgetinspector.h"
#include "ui_widgetinspector.h"

#include "overlaywidget.h"

#include <probeinterface.h>
#include <kde/krecursivefilterproxymodel.h>
#include <objecttypefilterproxymodel.h>
#include <objectmodel.h>

#include <QDesktopWidget>

using namespace GammaRay;

WidgetInspector::WidgetInspector(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent), ui(new Ui::WidgetInspector), m_overlayWidget(new OverlayWidget)
{
  ui->setupUi(this);

  m_overlayWidget->hide();

  connect(probe->probe(), SIGNAL(widgetSelected(QWidget*,QPoint)), SLOT(widgetSelected(QWidget*)));

  ObjectTypeFilterProxyModel<QWidget> *widgetFilterProxy =
    new ObjectTypeFilterProxyModel<QWidget>(this);
  widgetFilterProxy->setSourceModel(probe->objectTreeModel());
  KRecursiveFilterProxyModel *widgetSearchProxy = new KRecursiveFilterProxyModel(this);
  widgetSearchProxy->setSourceModel(widgetFilterProxy);
  ui->widgetTreeView->setModel(widgetSearchProxy);
  ui->widgetTreeView->header()->setResizeMode(0, QHeaderView::Stretch);
  ui->widgetTreeView->header()->setResizeMode(1, QHeaderView::Interactive);
  ui->widgetSearchLine->setProxy(widgetSearchProxy);
  connect(ui->widgetTreeView->selectionModel(),
          SIGNAL(currentChanged(QModelIndex,QModelIndex)),
          SLOT(widgetSelected(QModelIndex)));
}

void WidgetInspector::widgetSelected(const QModelIndex &index)
{
  if (index.isValid()) {
    QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject*>();
    ui->widgetPropertyWidget->setObject(obj);
    ui->widgetPreviewWidget->setWidget(qobject_cast<QWidget*>(obj));

    QWidget *widget = qobject_cast<QWidget*>(obj);
    if (widget && qobject_cast<QDesktopWidget*>(widget) == 0) {
      m_overlayWidget->placeOn(widget);
    } else {
      m_overlayWidget->placeOn(0);
    }
  } else {
    ui->widgetPropertyWidget->setObject(0);
    ui->widgetPreviewWidget->setWidget(0);
    m_overlayWidget->placeOn(0);
  }
}

void WidgetInspector::widgetSelected(QWidget *widget)
{
  QAbstractItemModel *model = ui->widgetTreeView->model();
  const QModelIndexList indexList =
    model->match(model->index(0, 0),
                 ObjectModel::ObjectRole,
                 QVariant::fromValue<QObject*>(widget), 1,
                 Qt::MatchExactly | Qt::MatchRecursive);
  if (indexList.isEmpty()) {
    return;
  }
  const QModelIndex index = indexList.first();
  ui->widgetTreeView->selectionModel()->select(
    index,
    QItemSelectionModel::Select | QItemSelectionModel::Clear |
    QItemSelectionModel::Rows | QItemSelectionModel::Current);
  ui->widgetTreeView->scrollTo(index);
  widgetSelected(index);
}

#include "widgetinspector.moc"
