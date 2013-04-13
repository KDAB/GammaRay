/*
  widgetinspector.cpp

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

#include "widgetinspector.h"
#include "config-gammaray.h"
#include "overlaywidget.h"
#include "paintbufferviewer.h"
#include "widgettreemodel.h"

#include <core/propertycontroller.h>
#include <common/network/objectbroker.h>

#include "include/objectmodel.h"
#include "include/objecttypefilterproxymodel.h"
#include "include/probeinterface.h"

#include "other/modelutils.h"

#include <QDebug>
#include <QDesktopWidget>
#include <QLayout>
#include <QItemSelectionModel>
#include <QPainter>
#include <QPixmap>
#include <QMainWindow>
#include <QtPlugin>

#ifdef HAVE_PRIVATE_QT_HEADERS
#include <private/qpaintbuffer_p.h> //krazy:exclude=camelcase
#endif

using namespace GammaRay;

WidgetInspector::WidgetInspector(ProbeInterface *probe, QObject *parent)
  : ObjectServer("com.kdab.GammaRay.WidgetInspector", parent), m_overlayWidget(new OverlayWidget),
  m_propertyController(new PropertyController("com.kdab.GammaRay.WidgetInspector", this))
{
  m_overlayWidget->hide();
  connect(m_overlayWidget, SIGNAL(destroyed(QObject*)),
          SLOT(handleOverlayWidgetDestroyed(QObject*)));

  connect(probe->probe(), SIGNAL(widgetSelected(QWidget*,QPoint)), SLOT(widgetSelected(QWidget*)));

  WidgetTreeModel *widgetFilterProxy = new WidgetTreeModel(this);
  widgetFilterProxy->setSourceModel(probe->objectTreeModel());
  probe->registerModel("com.kdab.GammaRay.WidgetTree", widgetFilterProxy);

  m_widgetSelectionModel = ObjectBroker::selectionModel(widgetFilterProxy);
  connect(m_widgetSelectionModel,
          SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(widgetSelected(QItemSelection)));

  // TODO this needs to be delayed until there actually is something to select
  selectDefaultItem();
}

static bool isMainWindowSubclassAcceptor(const QVariant &v)
{
  return qobject_cast<QMainWindow*>(v.value<QObject*>());
}

void WidgetInspector::selectDefaultItem()
{
  const QAbstractItemModel *viewModel = m_widgetSelectionModel->model();
  const QModelIndexList matches =
    ModelUtils::match(
      viewModel, viewModel->index(0, 0),
      ObjectModel::ObjectRole, isMainWindowSubclassAcceptor);

  if (!matches.isEmpty()) {
    m_widgetSelectionModel->select(matches.first(), QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
  }
}

void WidgetInspector::widgetSelected(const QItemSelection& selection)
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

    m_propertyController->setObject(obj);

    if (widget &&
        qobject_cast<QDesktopWidget*>(widget) == 0 &&
        !widget->inherits("QDesktopScreenWidget")) {
      m_overlayWidget->placeOn(widget);
    } else {
      m_overlayWidget->placeOn(0);
    }
  } else {
    m_propertyController->setObject(0);
    m_overlayWidget->placeOn(0);
  }
}

void WidgetInspector::handleOverlayWidgetDestroyed(QObject *)
{
  // the target application might have destroyed the overlay widget
  // (e.g. because the parent of the overlay got destroyed).
  // just recreate a new one in this case
  m_overlayWidget = new OverlayWidget;
  m_overlayWidget->hide();
}

void WidgetInspector::widgetSelected(QWidget *widget)
{
  const QAbstractItemModel *model = m_widgetSelectionModel->model();
  const QModelIndexList indexList =
    model->match(model->index(0, 0),
                 ObjectModel::ObjectRole,
                 QVariant::fromValue<QObject*>(widget), 1,
                 Qt::MatchExactly | Qt::MatchRecursive);
  if (indexList.isEmpty()) {
    return;
  }
  const QModelIndex index = indexList.first();
  m_widgetSelectionModel->select(
    index,
    QItemSelectionModel::Select | QItemSelectionModel::Clear |
    QItemSelectionModel::Rows | QItemSelectionModel::Current);
}

QWidget *WidgetInspector::selectedWidget() const
{
  const QModelIndexList indexes = m_widgetSelectionModel->selectedRows();
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

// TODO the following actions should actually store the file on the client!

void WidgetInspector::saveAsImage(const QString& fileName)
{
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

void WidgetInspector::saveAsSvg(const QString &fileName)
{
  QWidget *widget = selectedWidget();
  if (fileName.isEmpty() || !widget) {
    return;
  }

  m_overlayWidget->hide();
  callExternalExportAction("gammaray_save_widget_to_svg", widget, fileName);
  m_overlayWidget->show();
}

void WidgetInspector::saveAsPdf(const QString &fileName)
{
  QWidget *widget = selectedWidget();
  if (fileName.isEmpty() || !widget) {
    return;
  }

  m_overlayWidget->hide();
  callExternalExportAction("gammaray_save_widget_to_pdf", widget, fileName);
  m_overlayWidget->show();
}

void WidgetInspector::saveAsUiFile(const QString &fileName)
{
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

  void(*function)(QWidget *, const QString &) =
    reinterpret_cast<void(*)(QWidget *, const QString &)>(m_externalExportActions.resolve(name));

  if (!function) {
    qWarning() << m_externalExportActions.errorString();
    return;
  }
  function(widget, fileName);
}

void WidgetInspector::analyzePainting()
{
  qDebug() << Q_FUNC_INFO << selectedWidget();
  QWidget *widget = selectedWidget();
  if (!widget) {
    return;
  }
#ifdef HAVE_PRIVATE_QT_HEADERS
  QPaintBuffer buffer;
  m_overlayWidget->hide();
  buffer.setBoundingRect(widget->rect());
  widget->render(&buffer);
  m_overlayWidget->show();

  // TODO: this still needs a core/UI split to work remotely
  PaintBufferViewer *viewer = new PaintBufferViewer(0);
  viewer->setWindowTitle(tr("Analyze Painting"));
  viewer->setAttribute(Qt::WA_DeleteOnClose);
  viewer->setPaintBuffer(buffer);
  viewer->show();
#endif
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(WidgetInspectorFactory)
#endif

#include "widgetinspector.moc"
