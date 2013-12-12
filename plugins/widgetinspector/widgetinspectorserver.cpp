/*
  widgetinspectorserver.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#include "widgetinspectorserver.h"

#include "config-gammaray.h"
#include "overlaywidget.h"
#include "paintbufferviewer.h"
#include "widgettreemodel.h"
#include "paintbuffermodel.h"

#include <core/propertycontroller.h>
#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/varianthandler.h>
#include <core/probesettings.h>
#include <core/objecttypefilterproxymodel.h>
#include <core/probeinterface.h>

#include <common/objectbroker.h>
#include <common/settempvalue.h>
#include <common/metatypedeclarations.h>
#include <common/objectmodel.h>

#include "other/modelutils.h"

#include <QAbstractItemView>
#include <QApplication>
#include <QComboBox>
#include <QDesktopWidget>
#include <QLayout>
#include <QItemSelectionModel>
#include <QPainter>
#include <QPixmap>
#include <QMainWindow>
#include <QMouseEvent>
#include <QEvent>
#include <QTimer>
#include <QStyle>

#include <iostream>

#ifdef HAVE_PRIVATE_QT_HEADERS
#include <private/qpaintbuffer_p.h> //krazy:exclude=camelcase
#endif

Q_DECLARE_METATYPE(const QStyle *)

using namespace GammaRay;
using namespace std;

WidgetInspectorServer::WidgetInspectorServer(ProbeInterface *probe, QObject *parent)
  : WidgetInspectorInterface(parent)
  , m_overlayWidget(new OverlayWidget)
  , m_propertyController(new PropertyController(objectName(), this))
  , m_updatePreviewTimer(new QTimer(this))
  , m_paintBufferModel(0)
  , m_paintAnalyzerTimer(new QTimer(this))
  , m_probe(probe)
{
  registerWidgetMetaTypes();
  registerVariantHandlers();
  probe->installGlobalEventFilter(this);

  m_updatePreviewTimer->setSingleShot(true);
  m_updatePreviewTimer->setInterval(100);
  connect(m_updatePreviewTimer, SIGNAL(timeout()), SLOT(updateWidgetPreview()));

  m_paintAnalyzerTimer->setSingleShot(true);
  m_paintAnalyzerTimer->setInterval(100);
  connect(m_paintAnalyzerTimer, SIGNAL(timeout()), SLOT(updatePaintAnalyzer()));

  m_overlayWidget->hide();
  connect(m_overlayWidget, SIGNAL(destroyed(QObject*)),
          SLOT(handleOverlayWidgetDestroyed(QObject*)));

  WidgetTreeModel *widgetFilterProxy = new WidgetTreeModel(this);
  widgetFilterProxy->setSourceModel(probe->objectTreeModel());
  probe->registerModel("com.kdab.GammaRay.WidgetTree", widgetFilterProxy);

  m_widgetSelectionModel = ObjectBroker::selectionModel(widgetFilterProxy);
  connect(m_widgetSelectionModel,
          SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(widgetSelected(QItemSelection)));

#ifdef HAVE_PRIVATE_QT_HEADERS
  m_paintBufferModel = new PaintBufferModel(this);
  probe->registerModel("com.kdab.GammaRay.PaintBufferModel", m_paintBufferModel);
  connect(ObjectBroker::selectionModel(m_paintBufferModel), SIGNAL(currentChanged(QModelIndex,QModelIndex)),
          this, SLOT(eventuallyUpdatePaintAnalyzer()));
#endif

  // TODO this needs to be delayed until there actually is something to select
  selectDefaultItem();

  if (!m_probe->hasReliableObjectTracking()) {
    connect(m_probe->probe(), SIGNAL(objectCreated(QObject*)), SLOT(objectCreated(QObject*)));
    discoverObjects();
  }
}

static bool isMainWindowSubclassAcceptor(const QVariant &v)
{
  return qobject_cast<QMainWindow*>(v.value<QObject*>());
}

void WidgetInspectorServer::selectDefaultItem()
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

void WidgetInspectorServer::widgetSelected(const QItemSelection &selection)
{
  m_propertyController->setObject(0);

  if (selection.isEmpty())
    return;
  const QModelIndex index = selection.first().topLeft();

  QWidget *widget = 0;
  if (index.isValid()) {
    QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject*>();
    m_propertyController->setObject(obj);
    widget = qobject_cast<QWidget*>(obj);
    QLayout *layout = qobject_cast<QLayout*>(obj);
    if (!widget && layout) {
      widget = layout->parentWidget();
    }
  }

  if (m_selectedWidget == widget) {
    return;
  }

  m_selectedWidget = widget;

  if (m_selectedWidget &&
      (qobject_cast<QDesktopWidget*>(m_selectedWidget) ||
      m_selectedWidget->inherits("QDesktopScreenWidget"))) {
    m_overlayWidget->placeOn(0);
    return;
  }

  m_overlayWidget->placeOn(m_selectedWidget);

  if (!m_selectedWidget) {
    return;
  }

  updateWidgetPreview();
}

bool WidgetInspectorServer::eventFilter(QObject *object, QEvent *event)
{
  if (object == m_selectedWidget && event->type() == QEvent::Paint) {
    // delay pixmap grabbing such that the object can update itself beforehand
    // also use a timer to prevent aggregation of previews
    if (!m_updatePreviewTimer->isActive()) {
      m_updatePreviewTimer->start();
    }
  }

  // make modal dialogs non-modal so that the gammaray window is still reachable
  // TODO: should only be done in in-process mode
  if (event->type() == QEvent::Show) {
    QDialog *dlg = qobject_cast<QDialog*>(object);
    if (dlg) {
      dlg->setWindowModality(Qt::NonModal);
    }
  }

  if (event->type() == QEvent::MouseButtonRelease) {
    QMouseEvent *mouseEv = static_cast<QMouseEvent*>(event);
    if (mouseEv->button() == Qt::LeftButton &&
      mouseEv->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)) {
      QWidget *widget = QApplication::widgetAt(mouseEv->globalPos());
      if (widget) {
        // also select the corresponding model if a view was selected
        if (QAbstractItemView *view = Util::findParentOfType<QAbstractItemView>(object)) {
          m_probe->selectObject(view->model());
        } else if (QComboBox *box = Util::findParentOfType<QComboBox>(object)) {
          m_probe->selectObject(box->model());
        }

        m_probe->selectObject(widget, widget->mapFromGlobal(mouseEv->globalPos()));
        widgetSelected(widget);
      }
    }
  }

  return QObject::eventFilter(object, event);
}

void WidgetInspectorServer::updateWidgetPreview()
{
  if (!m_selectedWidget) {
    return;
  }

  emit widgetPreviewAvailable(pixmapForWidget(m_selectedWidget));
}

QPixmap WidgetInspectorServer::pixmapForWidget(QWidget *widget)
{
  // prevent "recursion", i.e. infinite update loop, in our eventFilter
  Util::SetTempValue<QPointer<QWidget> > guard(m_selectedWidget, 0);
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  return QPixmap::grabWidget(widget);
#else
  return widget->grab();
#endif
}

void WidgetInspectorServer::handleOverlayWidgetDestroyed(QObject *)
{
  // the target application might have destroyed the overlay widget
  // (e.g. because the parent of the overlay got destroyed).
  // just recreate a new one in this case
  m_overlayWidget = new OverlayWidget;
  m_overlayWidget->hide();
}

void WidgetInspectorServer::widgetSelected(QWidget *widget)
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

// TODO the following actions should actually store the file on the client!

void WidgetInspectorServer::saveAsImage(const QString& fileName)
{
  if (fileName.isEmpty() || !m_selectedWidget) {
    return;
  }

  m_overlayWidget->hide();
  QPixmap pixmap = pixmapForWidget(m_selectedWidget);
  m_overlayWidget->show();
  pixmap.save(fileName);
}

void WidgetInspectorServer::saveAsSvg(const QString &fileName)
{
  if (fileName.isEmpty() || !m_selectedWidget) {
    return;
  }

  m_overlayWidget->hide();
  callExternalExportAction("gammaray_save_widget_to_svg", m_selectedWidget, fileName);
  m_overlayWidget->show();
}

void WidgetInspectorServer::saveAsPdf(const QString &fileName)
{
  if (fileName.isEmpty() || !m_selectedWidget) {
    return;
  }

  m_overlayWidget->hide();
  callExternalExportAction("gammaray_save_widget_to_pdf", m_selectedWidget, fileName);
  m_overlayWidget->show();
}

void WidgetInspectorServer::saveAsUiFile(const QString &fileName)
{
  if (fileName.isEmpty() || !m_selectedWidget) {
    return;
  }

  callExternalExportAction("gammaray_save_widget_to_ui", m_selectedWidget, fileName);
}

void WidgetInspectorServer::callExternalExportAction(const char *name,
                                               QWidget *widget,
                                               const QString &fileName)
{
  if (!m_externalExportActions.isLoaded()) {
    const QString probePath = ProbeSettings::value("ProbePath").toString();

    m_externalExportActions.setFileName(
      probePath + QLatin1String("/libgammaray_widget_export_actions"));

    m_externalExportActions.load();
  }

  void(*function)(QWidget *, const QString &) =
    reinterpret_cast<void(*)(QWidget *, const QString &)>(m_externalExportActions.resolve(name));

  if (!function) {
    cerr << Q_FUNC_INFO << ' ' << qPrintable(m_externalExportActions.errorString()) << endl;
    return;
  }
  function(widget, fileName);
}

void WidgetInspectorServer::analyzePainting()
{
  if (!m_selectedWidget) {
    return;
  }
#ifdef HAVE_PRIVATE_QT_HEADERS
  QPaintBuffer buffer;
  m_overlayWidget->hide();
  buffer.setBoundingRect(m_selectedWidget->rect());
  m_selectedWidget->render(&buffer);
  m_overlayWidget->show();
  m_paintBufferModel->setPaintBuffer(buffer);
  eventuallyUpdatePaintAnalyzer();
#endif
}

void WidgetInspectorServer::eventuallyUpdatePaintAnalyzer()
{
#ifdef HAVE_PRIVATE_QT_HEADERS
  m_paintAnalyzerTimer->start();
#endif
}

void WidgetInspectorServer::updatePaintAnalyzer()
{
#ifdef HAVE_PRIVATE_QT_HEADERS
  // didn't manage painting on the widget directly, even with the correct
  // translation it is always clipping as if the widget was at 0,0 of its parent
  const QSize sourceSize = m_paintBufferModel->buffer().boundingRect().size().toSize();
  QPixmap pixmap(sourceSize);
  QPainter painter(&pixmap);
  Util::drawTransparencyPattern(&painter, QRect(QPoint(0, 0), sourceSize));
  int start = m_paintBufferModel->buffer().frameStartIndex(0);
  // include selected row or paint all if nothing is selected
  const QModelIndex index = ObjectBroker::selectionModel(m_paintBufferModel)->currentIndex();
  int end = index.isValid() ? index.row() + 1 : m_paintBufferModel->rowCount();
  int depth = m_paintBufferModel->buffer().processCommands(&painter, start, start + end);
  for (; depth > 0; --depth) {
    painter.restore();
  }
  painter.end();
  emit paintAnalyzed(pixmap);
#endif
}

void WidgetInspectorServer::checkFeatures()
{
  emit features(
#ifdef HAVE_QT_SVG
    true,
#else
    false,
#endif
#ifdef HAVE_QT_PRINTSUPPORT
    true,
#else
    false,
#endif
#ifdef HAVE_QT_DESIGNER
    true,
#else
    false,
#endif
#ifdef HAVE_PRIVATE_QT_HEADERS
    true
#else
    false
#endif
  );
}

void WidgetInspectorServer::registerWidgetMetaTypes()
{
  MetaObject *mo = 0;
  MO_ADD_METAOBJECT2(QWidget, QObject, QPaintDevice);
  MO_ADD_PROPERTY_RO(QWidget, QWidget*, focusProxy);

  MO_ADD_METAOBJECT1(QStyle, QObject);
  MO_ADD_PROPERTY_RO(QStyle, const QStyle*, proxy);
  MO_ADD_PROPERTY_RO(QStyle, QPalette, standardPalette);
}

static QString sizePolicyPolicyToString(QSizePolicy::Policy policy)
{
  const int index = QSizePolicy::staticMetaObject.indexOfEnumerator("Policy");
  const QMetaEnum metaEnum = QSizePolicy::staticMetaObject.enumerator(index);
  return QString::fromLatin1(metaEnum.valueToKey(policy));
}

static QString sizePolicyToString(const QSizePolicy &policy)
{
  return QString::fromLatin1("%1 x %2").
    arg(sizePolicyPolicyToString(policy.horizontalPolicy())).
    arg(sizePolicyPolicyToString(policy.verticalPolicy()));

}

void WidgetInspectorServer::registerVariantHandlers()
{
  VariantHandler::registerStringConverter<QSizePolicy>(sizePolicyToString);

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
  VariantHandler::registerStringConverter<QWidget*>(Util::displayString);
  VariantHandler::registerStringConverter<const QStyle*>(Util::displayString);
#endif
}

void WidgetInspectorServer::discoverObjects()
{
  if (qApp) {
    foreach (QWidget *widget, qApp->topLevelWidgets())
      m_probe->discoverObject(widget);
  }
}

void WidgetInspectorServer::objectCreated(QObject* object)
{
  if (!object)
    return;

  if (qobject_cast<QApplication*>(object))
    discoverObjects();
  if (QAbstractItemView* view = qobject_cast<QAbstractItemView*>(object))
    m_probe->discoverObject(view->model());
}
