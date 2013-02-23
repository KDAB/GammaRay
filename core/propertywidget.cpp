/*
  propertywidget.cpp

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

#include "propertywidget.h"
#include "ui_propertywidget.h"

#include "connectionfilterproxymodel.h"
#include "connectionmodel.h"
#include "methodinvocationdialog.h"
#include "objectmethodmodel.h"
#include "probe.h"
#include "proxydetacher.h"

#include "propertyeditor/propertyeditorfactory.h"
#include <ui/deferredresizemodesetter.h>

#include <network/objectbroker.h>
#include <network/networkobject.h>

#include "kde/krecursivefilterproxymodel.h"

#include <QDebug>
#include <QMenu>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QTime>

using namespace GammaRay;

static bool removePage(QTabWidget *tabWidget, QWidget *widget)
{
  const int index = tabWidget->indexOf(widget);
  if (index == -1) {
    return false;
  }

  tabWidget->removeTab(index);
  return true;
}

PropertyWidget::PropertyWidget(QWidget *parent)
  : QWidget(parent),
    m_ui(new Ui_PropertyWidget),
    m_inboundConnectionModel(new ConnectionFilterProxyModel(this)),
    m_outboundConnectionModel(new ConnectionFilterProxyModel(this)),
    m_editorFactory(new PropertyEditorFactory),
    m_displayState(PropertyWidgetDisplayState::QObject)
{
  m_ui->setupUi(this);
}

PropertyWidget::~PropertyWidget()
{
}

void PropertyWidget::setObjectBaseName(const QString& baseName)
{
  m_objectBaseName = baseName;

  QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(model("staticProperties"));
  m_ui->staticPropertyView->setModel(proxy);
  m_ui->staticPropertyView->sortByColumn(0, Qt::AscendingOrder);
  new DeferredResizeModeSetter(m_ui->staticPropertyView->header(), 0, QHeaderView::ResizeToContents);
  m_ui->staticPropertySearchLine->setProxy(proxy);
  setEditorFactory(m_ui->staticPropertyView);

  proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(model("dynamicProperties"));
  m_ui->dynamicPropertyView->setModel(proxy);
  m_ui->dynamicPropertyView->sortByColumn(0, Qt::AscendingOrder);
  new DeferredResizeModeSetter(m_ui->dynamicPropertyView->header(), 0, QHeaderView::ResizeToContents);
  setEditorFactory(m_ui->dynamicPropertyView);
  m_ui->dynamicPropertySearchLine->setProxy(proxy);

  proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(model("methods"));
  m_ui->methodView->setModel(proxy);
  m_ui->methodView->sortByColumn(0, Qt::AscendingOrder);
  m_ui->methodView->setSelectionModel(ObjectBroker::selectionModel(proxy));
  m_ui->methodView->header()->setResizeMode(QHeaderView::ResizeToContents);
  m_ui->methodSearchLine->setProxy(proxy);
  connect(m_ui->methodView, SIGNAL(doubleClicked(QModelIndex)),
          SLOT(methodActivated(QModelIndex)));
  connect(m_ui->methodView, SIGNAL(customContextMenuRequested(QPoint)),
          SLOT(methodConextMenu(QPoint)));
  m_ui->methodLog->setModel(model("methodLog"));

  proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(model("classInfo"));
  m_ui->classInfoView->setModel(proxy);
  m_ui->classInfoView->sortByColumn(0, Qt::AscendingOrder);
  m_ui->classInfoView->header()->setResizeMode(QHeaderView::ResizeToContents);
  m_ui->classInfoSearchLine->setProxy(proxy);

#ifndef GAMMARAY_CLIENT
  if (Probe::isInitialized()) {
    new ProxyDetacher(m_ui->inboundConnectionView, m_inboundConnectionModel,
                      Probe::instance()->connectionModel());
    m_ui->inboundConnectionView->setModel(m_inboundConnectionModel);
    m_ui->inboundConnectionView->sortByColumn(0, Qt::AscendingOrder);
    m_ui->inboundConnectionSearchLine->setProxy(m_inboundConnectionModel);

    new ProxyDetacher(m_ui->outboundConnectionView, m_outboundConnectionModel,
                      Probe::instance()->connectionModel());
    m_ui->outboundConnectionView->setModel(m_outboundConnectionModel);
    m_ui->outboundConnectionView->sortByColumn(0, Qt::AscendingOrder);
    m_ui->outboundConnectionSearchLine->setProxy(m_outboundConnectionModel);
  } else {
    qDebug() << "Probe not initialized, disabling connection tabs";
    removePage(m_ui->tabWidget, m_ui->inboundConnectionTab);
    removePage(m_ui->tabWidget, m_ui->outboundConnectionTab);
  }
#else
  m_ui->inboundConnectionView->setModel(model("inboundConnections"));
  m_ui->outboundConnectionView->setModel(model("outboundConnections"));
#endif

  proxy = new KRecursiveFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(model("enums"));
  m_ui->enumView->setModel(proxy);
  m_ui->enumView->sortByColumn(0, Qt::AscendingOrder);
  m_ui->enumView->header()->setResizeMode(QHeaderView::ResizeToContents);
  m_ui->enumSearchLine->setProxy(proxy);

  // save back initial tab widgets
  for (int i = 0; i < m_ui->tabWidget->count(); ++i) {
    m_tabWidgets.push_back(qMakePair(m_ui->tabWidget->widget(i), m_ui->tabWidget->tabText(i)));
  }

  proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(model("nonQProperties"));
  m_ui->metaPropertyView->setModel(proxy);
  m_ui->metaPropertyView->sortByColumn(0, Qt::AscendingOrder);
  m_ui->metaPropertySearchLine->setProxy(proxy);
  setEditorFactory(m_ui->metaPropertyView);

  ObjectBroker::object(m_objectBaseName + ".controller")->subscribeToSignal("displayState", this, "setDisplayState");
}

QAbstractItemModel* PropertyWidget::model(const QString& nameSuffix)
{
  return ObjectBroker::model(m_objectBaseName + "." + nameSuffix);
}

void GammaRay::PropertyWidget::setObject(QObject *object)
{
  m_object = object;
  m_inboundConnectionModel->filterReceiver(object);
  m_outboundConnectionModel->filterSender(object);
}

void PropertyWidget::setObject(void *object, const QString &className)
{
  setObject(0);
}

void PropertyWidget::setMetaObject(const QMetaObject *metaObject)
{
  setObject(0);
}

void GammaRay::PropertyWidget::methodActivated(const QModelIndex &index)
{
  ObjectBroker::object(m_objectBaseName + ".controller")->emitSignal("activateMethod");

  const QMetaMethod::MethodType methodType = index.data(ObjectMethodModelRole::MetaMethodType).value<QMetaMethod::MethodType>();
  if (methodType == QMetaMethod::Slot) {
    MethodInvocationDialog dlg(this);
    dlg.setArgumentModel(model("methodArguments"));
    if (dlg.exec()) {
      ObjectBroker::object(m_objectBaseName + ".controller")->emitSignal("invokeMethod", QVariantList() << QVariant::fromValue(dlg.connectionType()));
    }
  }
}

void PropertyWidget::methodConextMenu(const QPoint &pos)
{
  const QModelIndex index = m_ui->methodView->indexAt(pos);
  if (!index.isValid() || m_displayState != PropertyWidgetDisplayState::QObject) {
    return;
  }

  const QMetaMethod::MethodType methodType = index.data(ObjectMethodModelRole::MetaMethodType).value<QMetaMethod::MethodType>();
  QMenu contextMenu;
  if (methodType == QMetaMethod::Slot) {
    contextMenu.addAction(tr("Invoke"));
  } else if (methodType == QMetaMethod::Signal) {
    contextMenu.addAction(tr("Connect to"));
  }

  if (contextMenu.exec(m_ui->methodView->viewport()->mapToGlobal(pos))) {
    methodActivated(index);
  }
}

bool PropertyWidget::showTab(const QWidget* widget, PropertyWidgetDisplayState::State state) const
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  if (widget == m_ui->inboundConnectionTab ||
      widget == m_ui->outboundConnectionTab) {
    return false;
  }
#endif
  switch(state) {
  case PropertyWidgetDisplayState::QObject:
    return true; // show all
  case PropertyWidgetDisplayState::Object:
    if (widget == m_ui->metaPropertyTab) {
      return true;
    }
    break;
  case PropertyWidgetDisplayState::MetaObject:
    if (widget == m_ui->enumTab || widget == m_ui->classInfoTab || widget == m_ui->methodTab) {
      return true;
    }
    break;
  }
  return false;
}

void PropertyWidget::setDisplayState(PropertyWidgetDisplayState::State state)
{
  m_displayState = state;
  QWidget *currentWidget = m_ui->tabWidget->currentWidget();

  // iterate through all tabs, decide for each tab if it gets hidden or not
  typedef QPair<QWidget *, QString> WidgetStringPair;
  Q_FOREACH (const WidgetStringPair &tab, m_tabWidgets) {
    const bool show = showTab(tab.first, state);
    if (show) {
      m_ui->tabWidget->addTab(tab.first, tab.second);
    } else {
      removePage(m_ui->tabWidget, tab.first);
    }
  }

  if (m_ui->tabWidget->indexOf(currentWidget) >= 0) {
    m_ui->tabWidget->setCurrentWidget(currentWidget);
  }

  m_ui->methodLog->setVisible(m_displayState == PropertyWidgetDisplayState::QObject);
}

void PropertyWidget::setEditorFactory(QAbstractItemView *view)
{
  QStyledItemDelegate *delegate = qobject_cast<QStyledItemDelegate*>(view->itemDelegate());
  if (delegate) {
    delegate->setItemEditorFactory(m_editorFactory.data());
  }
}

#include "propertywidget.moc"
