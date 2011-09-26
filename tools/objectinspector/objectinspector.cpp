/*
  objectinspector.cpp

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

#include "objectinspector.h"
#include "ui_objectinspector.h"

#include <kde/krecursivefilterproxymodel.h>
#include <probeinterface.h>
#include <objectlistmodel.h>

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
  ui->objectSearchLine->setProxy(objectFilter);
  connect(ui->objectTreeView->selectionModel(),
          SIGNAL(currentChanged(QModelIndex,QModelIndex)),
          SLOT(objectSelected(QModelIndex)));

  if (qgetenv("GAMMARAY_TEST_FILTER") == "1") {
    QMetaObject::invokeMethod(ui->objectSearchLine->lineEdit(), "setText",
                              Qt::QueuedConnection,
                              Q_ARG(QString, QLatin1String("Object")));
  }
}

void ObjectInspector::objectSelected(const QModelIndex &index)
{
  if (index.isValid()) {
    QObject *obj = index.data(ObjectListModel::ObjectRole).value<QObject*>();
    ui->objectPropertyWidget->setObject(obj);
  } else {
    ui->objectPropertyWidget->setObject(0);
  }
}

#include "objectinspector.moc"
