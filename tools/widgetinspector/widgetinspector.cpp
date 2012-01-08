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
#include "widgettreemodel.h"

#include <probeinterface.h>
#include <kde/krecursivefilterproxymodel.h>
#include <objecttypefilterproxymodel.h>
#include <objectmodel.h>

#include <QDebug>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QPainter>
#include <QPixmap>
#include <QPrinter>

using namespace GammaRay;

WidgetInspector::WidgetInspector(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent), ui(new Ui::WidgetInspector), m_overlayWidget(new OverlayWidget)
{
  ui->setupUi(this);

  m_overlayWidget->hide();

  connect(probe->probe(), SIGNAL(widgetSelected(QWidget*,QPoint)), SLOT(widgetSelected(QWidget*)));

  WidgetTreeModel *widgetFilterProxy = new WidgetTreeModel(this);
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

  connect(ui->actionSaveAsImage, SIGNAL(triggered()), SLOT(saveAsImage()));
  connect(ui->actionSaveAsSvg, SIGNAL(triggered()), SLOT(saveAsSvg()));
  connect(ui->actionSaveAsPdf, SIGNAL(triggered()), SLOT(saveAsPdf()));
  connect(ui->actionSaveAsUiFile, SIGNAL(triggered()), SLOT(saveAsUiFile()));

  addAction(ui->actionSaveAsImage);
  addAction(ui->actionSaveAsSvg);
  addAction(ui->actionSaveAsPdf);
  addAction(ui->actionSaveAsUiFile);

  setActionsEnabled(false);
}

void WidgetInspector::widgetSelected(const QModelIndex &index)
{
  if (index.isValid()) {
    QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject*>();
    QWidget *widget = qobject_cast<QWidget*>(obj);
    QLayout* layout = qobject_cast<QLayout*>(obj);
    if (!widget && layout) {
      widget = layout->parentWidget();
    }

    ui->widgetPropertyWidget->setObject(obj);
    ui->widgetPreviewWidget->setWidget(widget);
    setActionsEnabled(widget != 0);

    if (widget && qobject_cast<QDesktopWidget*>(widget) == 0) {
      m_overlayWidget->placeOn(widget);
    } else {
      m_overlayWidget->placeOn(0);
    }
  } else {
    ui->widgetPropertyWidget->setObject(0);
    ui->widgetPreviewWidget->setWidget(0);
    m_overlayWidget->placeOn(0);
    setActionsEnabled(false);
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

void WidgetInspector::setActionsEnabled(bool enabled)
{
  foreach (QAction *action, actions())
    action->setEnabled(enabled);
}

QWidget *WidgetInspector::selectedWidget() const
{
  const QModelIndexList indexes = ui->widgetTreeView->selectionModel()->selectedRows();
  if (indexes.isEmpty()) {
    return 0;
  }
  const QModelIndex index = indexes.first();
  if (index.isValid()) {
    QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject*>();
    QWidget *widget = qobject_cast<QWidget*>(obj);
    QLayout* layout = qobject_cast<QLayout*>(obj);
    if (!widget && layout) {
      widget = layout->parentWidget();
    }
    return widget;
  }
  return 0;
}

void WidgetInspector::saveAsImage()
{
  const QString fileName =
    QFileDialog::getSaveFileName(
      this,
      tr("Save As Image"),
      QString(),
      tr("Image Files (*.png *.jpg)"));

  QWidget *widget = selectedWidget();
  if (fileName.isEmpty() || !widget) {
    return;
  }

  QPixmap pixmap(widget->size());
  m_overlayWidget->hide();
  widget->render(&pixmap);
  m_overlayWidget->show();
  pixmap.save(fileName);
}

void WidgetInspector::saveAsSvg()
{
  const QString fileName =
    QFileDialog::getSaveFileName(
      this,
      tr("Save As SVG"),
      QString(),
      tr("Scalable Vector Graphics (*.svg)"));

  QWidget *widget = selectedWidget();
  if (fileName.isEmpty() || !widget) {
    return;
  }

  m_overlayWidget->hide();
  callExternalExportAction("gammaray_save_widget_to_svg", widget, fileName);
  m_overlayWidget->show();
}

void WidgetInspector::saveAsPdf()
{
  const QString fileName =
    QFileDialog::getSaveFileName(
      this,
      tr("Save As PDF"),
      QString(),
      tr("PDF (*.pdf)"));

  QWidget *widget = selectedWidget();
  if (fileName.isEmpty() || !widget) {
    return;
  }

  QPrinter printer(QPrinter::ScreenResolution);
  printer.setOutputFileName(fileName);
  printer.setOutputFormat(QPrinter::PdfFormat);
  printer.setPageMargins(0, 0, 0, 0, QPrinter::DevicePixel);
  printer.setPaperSize(widget->size(), QPrinter::DevicePixel);

  m_overlayWidget->hide();
  widget->render(&printer);
  m_overlayWidget->show();
}

void WidgetInspector::saveAsUiFile()
{
  const QString fileName =
    QFileDialog::getSaveFileName(
      this,
      tr("Save As Qt Designer UI File"),
      QString(),
      tr("Qt Designer UI File (*.ui)"));

  QWidget *widget = selectedWidget();
  if (fileName.isEmpty() || !widget) {
    return;
  }

  callExternalExportAction("gammaray_save_widget_to_ui", widget, fileName);
}

void WidgetInspector::callExternalExportAction(const char *name,
                                               QWidget *widget,
                                               const QString &fileName)
{
  if (!m_externalExportActions.isLoaded()) {
    const QString probePath =
      QString::fromLocal8Bit(qgetenv("GAMMARAY_PROBE_PATH"));

    m_externalExportActions.setFileName(
      probePath + QLatin1String("/libgammaray_widget_export_actions"));

    m_externalExportActions.load();
  }

  void(*function)(QWidget*, const QString&) =
    reinterpret_cast<void(*)(QWidget*, const QString&)>(m_externalExportActions.resolve(name));

  if (!function) {
    qWarning() << m_externalExportActions.errorString();
    return;
  }
  function(widget, fileName);
}

#include "widgetinspector.moc"
