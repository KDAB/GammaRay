/*
  propertywidget.cpp

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

#include "propertywidget.h"
#include "ui_propertywidget.h"

#include "connectionfilterproxymodel.h"
#include "connectionmodel.h"
#include "metapropertymodel.h"
#include "methodinvocationdialog.h"
#include "multisignalmapper.h"
#include "objectclassinfomodel.h"
#include "objectdynamicpropertymodel.h"
#include "objectenummodel.h"
#include "objectmethodmodel.h"
#include "objectstaticpropertymodel.h"
#include "probe.h"
#include "proxydetacher.h"

#include "propertyeditor/propertyeditorfactory.h"

#include "kde/krecursivefilterproxymodel.h"

#include <QDebug>
#include <QMenu>
#include <QStandardItemModel>
#include <QStyledItemDelegate>
#include <QTime>

using namespace GammaRay;

static bool removePage(QTabWidget* tabWidget, QWidget* widget)
{
  const int index = tabWidget->indexOf(widget);
  Q_ASSERT(index >= 0);
  if (index == -1)
    return false;

  tabWidget->removeTab(index);
  return true;
}

PropertyWidget::PropertyWidget(QWidget *parent)
  : QWidget(parent),
    m_ui(new Ui_PropertyWidget),
    m_staticPropertyModel(new ObjectStaticPropertyModel(this)),
    m_dynamicPropertyModel(new ObjectDynamicPropertyModel(this)),
    m_classInfoModel(new ObjectClassInfoModel(this)),
    m_methodModel(new ObjectMethodModel(this)),
    m_inboundConnectionModel(new ConnectionFilterProxyModel(this)),
    m_outboundConnectionModel(new ConnectionFilterProxyModel(this)),
    m_enumModel(new ObjectEnumModel(this)),
    m_signalMapper(0),
    m_methodLogModel(new QStandardItemModel(this)),
    m_metaPropertyModel(new MetaPropertyModel(this)),
    m_editorFactory(new PropertyEditorFactory)
{
  m_ui->setupUi(this);

  QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(m_staticPropertyModel);
  m_ui->staticPropertyView->setModel(proxy);
  m_ui->staticPropertyView->sortByColumn(0, Qt::AscendingOrder);
  m_ui->staticPropertyView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
  m_ui->staticPropertySearchLine->setProxy(proxy);
  setEditorFactory(m_ui->staticPropertyView);

  proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(m_dynamicPropertyModel);
  m_ui->dynamicPropertyView->setModel(proxy);
  m_ui->dynamicPropertyView->sortByColumn(0, Qt::AscendingOrder);
  m_ui->dynamicPropertyView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
  setEditorFactory(m_ui->dynamicPropertyView);
  m_ui->dynamicPropertySearchLine->setProxy(proxy);

  proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(m_methodModel);
  m_ui->methodView->setModel(proxy);
  m_ui->methodView->sortByColumn(0, Qt::AscendingOrder);
  m_ui->methodView->header()->setResizeMode(QHeaderView::ResizeToContents);
  m_ui->methodSearchLine->setProxy(proxy);
  connect(m_ui->methodView, SIGNAL(doubleClicked(QModelIndex)),
          SLOT(methodActivated(QModelIndex)));
  connect(m_ui->methodView, SIGNAL(customContextMenuRequested(QPoint)),
          SLOT(methodConextMenu(QPoint)));
  m_ui->methodLog->setModel(m_methodLogModel);

  proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(m_classInfoModel);
  m_ui->classInfoView->setModel(proxy);
  m_ui->classInfoView->sortByColumn(0, Qt::AscendingOrder);
  m_ui->classInfoView->header()->setResizeMode(QHeaderView::ResizeToContents);
  m_ui->classInfoSearchLine->setProxy(proxy);

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
  }
  else {
    qDebug() << "Probe not initialized, disabling connection tabs";
    removePage(m_ui->tabWidget, m_ui->inboundConnectionTab);
    removePage(m_ui->tabWidget, m_ui->outboundConnectionTab);
  }

  proxy = new KRecursiveFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(m_enumModel);
  m_ui->enumView->setModel(proxy);
  m_ui->enumView->sortByColumn(0, Qt::AscendingOrder);
  m_ui->enumView->header()->setResizeMode(QHeaderView::ResizeToContents);
  m_ui->enumSearchLine->setProxy(proxy);

  m_ui->metaPropertyView->setModel(m_metaPropertyModel);
  setEditorFactory(m_ui->metaPropertyView);
}

PropertyWidget::~PropertyWidget()
{
}

void GammaRay::PropertyWidget::setObject(QObject *object)
{
  m_object = object;
  m_staticPropertyModel->setObject(object);
  m_dynamicPropertyModel->setObject(object);
  m_inboundConnectionModel->filterReceiver(object);
  m_outboundConnectionModel->filterSender(object);

  const QMetaObject* metaObject = 0;
  if (object)
    metaObject = object->metaObject();
  m_enumModel->setMetaObject(metaObject);
  m_classInfoModel->setMetaObject(metaObject);
  m_methodModel->setMetaObject(metaObject);

  delete m_signalMapper;
  m_signalMapper = new MultiSignalMapper(this);
  connect(m_signalMapper, SIGNAL(signalEmitted(QObject*,int)), SLOT(signalEmitted(QObject*,int)));

  m_methodLogModel->clear();

  m_metaPropertyModel->setObject(object);

  setQObjectTabsVisible(true);
}

void PropertyWidget::setObject(void *object, const QString &className)
{
  setObject(0);
  m_metaPropertyModel->setObject(object, className);
  setQObjectTabsVisible(false);
}

void GammaRay::PropertyWidget::methodActivated(const QModelIndex &index)
{
  const QMetaMethod method = index.data(ObjectMethodModel::MetaMethodRole).value<QMetaMethod>();
  if (method.methodType() == QMetaMethod::Slot) {
    MethodInvocationDialog *dlg = new MethodInvocationDialog(this);
    dlg->setMethod(m_object.data(), method);
    dlg->show();
    // TODO: return value should go into ui->methodLog
  } else if (method.methodType() == QMetaMethod::Signal) {
    m_signalMapper->connectToSignal(m_object, method);
  }
}

void PropertyWidget::signalEmitted(QObject *sender, int signalIndex)
{
  Q_ASSERT(m_object == sender);
  m_methodLogModel->appendRow(
    new QStandardItem(tr("%1: Signal %2 emitted").
                      arg(QTime::currentTime().toString("HH:mm:ss.zzz")).
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
                      arg(sender->metaObject()->method(signalIndex).signature())));
#else
                      arg(QString(sender->metaObject()->method(signalIndex).methodSignature()))));
#endif
}

void PropertyWidget::methodConextMenu(const QPoint &pos)
{
  const QModelIndex index = m_ui->methodView->indexAt(pos);
  if (!index.isValid()) {
    return;
  }

  const QMetaMethod method = index.data(ObjectMethodModel::MetaMethodRole).value<QMetaMethod>();
  QMenu contextMenu;
  if (method.methodType() == QMetaMethod::Slot) {
    contextMenu.addAction(tr("Invoke"));
  } else if (method.methodType() == QMetaMethod::Signal) {
    contextMenu.addAction(tr("Connect to"));
  }

  if (contextMenu.exec(m_ui->methodView->viewport()->mapToGlobal(pos))) {
    methodActivated(index);
  }
}

void PropertyWidget::setQObjectTabsVisible(bool visible)
{
  // TODO: this should actually hide instead of disable...
  for (int i = 0; i < m_ui->tabWidget->count(); ++i) {
    if (m_ui->tabWidget->widget(i) != m_ui->metaPropertyTab) {
      m_ui->tabWidget->setTabEnabled(i, visible);
    }
  }
  if (!visible) {
    m_ui->tabWidget->setCurrentWidget(m_ui->metaPropertyTab);
  }
}

void PropertyWidget::setEditorFactory(QAbstractItemView *view)
{
  QStyledItemDelegate *delegate = qobject_cast<QStyledItemDelegate*>(view->itemDelegate());
  if (delegate) {
    delegate->setItemEditorFactory(m_editorFactory.data());
  }
}

#include "propertywidget.moc"
