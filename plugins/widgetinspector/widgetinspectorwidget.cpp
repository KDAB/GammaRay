/*
  widgetinspectorwidget.cpp

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

#include "widgetinspectorwidget.h"
#include "config-gammaray.h"
#include "overlaywidget.h"
#include "paintbufferviewer.h"
#include "widgettreemodel.h"
#include "ui_widgetinspectorwidget.h"

#include <core/propertycontroller.h>
#include <common/network/objectbroker.h>

#include "include/objectmodel.h"
#include "include/objecttypefilterproxymodel.h"
#include "include/probeinterface.h"

#include "kde/krecursivefilterproxymodel.h"
#include "other/modelutils.h"
#include <deferredresizemodesetter.h>

#include <QDebug>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QPainter>
#include <QPixmap>
#include <QMainWindow>
#include <QtPlugin>

#ifdef HAVE_PRIVATE_QT_HEADERS
#include <private/qpaintbuffer_p.h> //krazy:exclude=camelcase
#endif

using namespace GammaRay;

WidgetInspectorWidget::WidgetInspectorWidget(QWidget *parent)
  : QWidget(parent), ui(new Ui::WidgetInspectorWidget)
{
  ui->setupUi(this);
  ui->widgetPropertyWidget->setObjectBaseName("com.kdab.GammaRay.WidgetInspector");

  KRecursiveFilterProxyModel *widgetSearchProxy = new KRecursiveFilterProxyModel(this);
  widgetSearchProxy->setSourceModel(ObjectBroker::model("com.kdab.GammaRay.WidgetTree"));
  ui->widgetTreeView->setModel(widgetSearchProxy);
  ui->widgetTreeView->setSelectionModel(ObjectBroker::selectionModel(widgetSearchProxy));
  new DeferredResizeModeSetter(ui->widgetTreeView->header(), 0, QHeaderView::Stretch);
  new DeferredResizeModeSetter(ui->widgetTreeView->header(), 1, QHeaderView::Interactive);
  ui->widgetSearchLine->setProxy(widgetSearchProxy);
  connect(ui->widgetTreeView->selectionModel(),
          SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(widgetSelected(QItemSelection)));

  connect(ui->actionSaveAsImage, SIGNAL(triggered()), SLOT(saveAsImage()));
  connect(ui->actionSaveAsSvg, SIGNAL(triggered()), SLOT(saveAsSvg()));
  connect(ui->actionSaveAsPdf, SIGNAL(triggered()), SLOT(saveAsPdf()));
  connect(ui->actionSaveAsUiFile, SIGNAL(triggered()), SLOT(saveAsUiFile()));
  connect(ui->actionAnalyzePainting, SIGNAL(triggered()), SLOT(analyzePainting()));

  addAction(ui->actionSaveAsImage);
// TODO these checks needs to be dynamic, based on probe features
#ifdef HAVE_QT_SVG
  addAction(ui->actionSaveAsSvg);
#endif
#ifdef HAVE_QT_PRINTSUPPORT
  addAction(ui->actionSaveAsPdf);
#endif
#ifdef HAVE_QT_DESIGNER
  addAction(ui->actionSaveAsUiFile);
#endif
#ifdef HAVE_PRIVATE_QT_HEADERS
  addAction(ui->actionAnalyzePainting);
#endif

  setActionsEnabled(false);
}

void WidgetInspectorWidget::widgetSelected(const QItemSelection& selection)
{
  QModelIndex index;
  if (selection.size() > 0)
    index = selection.first().topLeft();

  if (index.isValid()) {
    QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject*>();
    QWidget *widget = qobject_cast<QWidget*>(obj);
    QLayout *layout = qobject_cast<QLayout*>(obj);
    if (!widget && layout) {
      widget = layout->parentWidget();
    }

    ui->widgetPreviewWidget->setWidget(widget);
    setActionsEnabled(true);

    // in case selection was triggered remotely
    ui->widgetTreeView->scrollTo(index);
  } else {
    ui->widgetPreviewWidget->setWidget(0);
    setActionsEnabled(false);
  }
}

void WidgetInspectorWidget::setActionsEnabled(bool enabled)
{
  foreach (QAction *action, actions()) {
    action->setEnabled(enabled);
  }
}

void WidgetInspectorWidget::saveAsImage()
{
  const QString fileName =
    QFileDialog::getSaveFileName(
      this,
      tr("Save As Image"),
      QString(),
      tr("Image Files (*.png *.jpg)"));

  if (fileName.isEmpty())
    return;
  ObjectBroker::object("com.kdab.GammaRay.WidgetInspector")->emitSignal("saveAsImage", QVariantList() << fileName);
}

void WidgetInspectorWidget::saveAsSvg()
{
  const QString fileName =
    QFileDialog::getSaveFileName(
      this,
      tr("Save As SVG"),
      QString(),
      tr("Scalable Vector Graphics (*.svg)"));

  if (fileName.isEmpty())
    return;
  ObjectBroker::object("com.kdab.GammaRay.WidgetInspector")->emitSignal("saveAsSvg", QVariantList() << fileName);
}

void WidgetInspectorWidget::saveAsPdf()
{
  const QString fileName =
    QFileDialog::getSaveFileName(
      this,
      tr("Save As PDF"),
      QString(),
      tr("PDF (*.pdf)"));

  if (fileName.isEmpty())
    return;
  ObjectBroker::object("com.kdab.GammaRay.WidgetInspector")->emitSignal("saveAsPdf", QVariantList() << fileName);
}

void WidgetInspectorWidget::saveAsUiFile()
{
  const QString fileName =
    QFileDialog::getSaveFileName(
      this,
      tr("Save As Qt Designer UI File"),
      QString(),
      tr("Qt Designer UI File (*.ui)"));

  if (fileName.isEmpty())
    return;
  ObjectBroker::object("com.kdab.GammaRay.WidgetInspector")->emitSignal("saveAsUiFile", QVariantList() << fileName);
}

void WidgetInspectorWidget::analyzePainting()
{
  qDebug() << Q_FUNC_INFO << ObjectBroker::object("com.kdab.GammaRay.WidgetInspector");
  ObjectBroker::object("com.kdab.GammaRay.WidgetInspector")->emitSignal("analyzePainting");
}

#include "widgetinspectorwidget.moc"
