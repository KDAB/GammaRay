/*
  objectinspector.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

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

#include "objectinspectorwidget.h"
#include "ui_objectinspectorwidget.h"

#include <common/objectbroker.h>

#include <ui/deferredresizemodesetter.h>
#include <ui/searchlinecontroller.h>

#include <QLineEdit>
#include <QItemSelectionModel>
#include <QTimer>

using namespace GammaRay;

ObjectInspectorWidget::ObjectInspectorWidget(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::ObjectInspectorWidget)
{
  ui->setupUi(this);
  ui->objectPropertyWidget->setObjectBaseName(QStringLiteral("com.kdab.GammaRay.ObjectInspector"));

  auto model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ObjectInspectorTree"));
  ui->objectTreeView->setModel(model);
  new DeferredResizeModeSetter(ui->objectTreeView->header(), 0, QHeaderView::Stretch);
  new DeferredResizeModeSetter(ui->objectTreeView->header(), 1, QHeaderView::Interactive);
  new SearchLineController(ui->objectSearchLine, model);

  QItemSelectionModel* selectionModel = ObjectBroker::selectionModel(ui->objectTreeView->model());
  ui->objectTreeView->setSelectionModel(selectionModel);
  connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          this, SLOT(objectSelectionChanged(QItemSelection)));

  if (qgetenv("GAMMARAY_TEST_FILTER") == "1") {
    QMetaObject::invokeMethod(ui->objectSearchLine, "setText",
                              Qt::QueuedConnection,
                              Q_ARG(QString, QStringLiteral("Object")));
  }
}

ObjectInspectorWidget::~ObjectInspectorWidget()
{
}

void ObjectInspectorWidget::objectSelectionChanged(const QItemSelection& selection)
{
  if (selection.isEmpty())
    return;
  const QModelIndex index = selection.first().topLeft();
  ui->objectTreeView->scrollTo(index);
}
