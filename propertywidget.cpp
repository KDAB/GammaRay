/*
  propertywidget.cpp

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

#include "propertywidget.h"
#include "objectstaticpropertymodel.h"
#include "objectdynamicpropertymodel.h"
#include "objectclassinfomodel.h"
#include "objectmethodmodel.h"
#include "objectenummodel.h"
#include "connectionmodel.h"
#include "connectionfilterproxymodel.h"
#include "probe.h"
#include "methodinvocationdialog.h"
#include "multisignalmapper.h"

#include "kde/krecursivefilterproxymodel.h"

#include <QDebug>
#include <QStandardItemModel>
#include <QtCore/QTime>
#include <qmenu.h>

using namespace GammaRay;

PropertyWidget::PropertyWidget(QWidget *parent)
  : QWidget(parent),
    m_staticPropertyModel(new ObjectStaticPropertyModel(this)),
    m_dynamicPropertyModel(new ObjectDynamicPropertyModel(this)),
    m_classInfoModel(new ObjectClassInfoModel(this)),
    m_methodModel(new ObjectMethodModel(this)),
    m_inboundConnectionModel(new ConnectionFilterProxyModel(this)),
    m_outboundConnectionModel(new ConnectionFilterProxyModel(this)),
    m_enumModel(new ObjectEnumModel(this)),
    m_signalMapper(0),
    m_methodLogModel(new QStandardItemModel(this))
{
  ui.setupUi(this);

  QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(m_staticPropertyModel);
  ui.staticPropertyView->setModel(proxy);
  ui.staticPropertyView->sortByColumn(0, Qt::AscendingOrder);
  ui.staticPropertyView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
  ui.staticPropertySearchLine->setProxy(proxy);

  proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(m_dynamicPropertyModel);
  ui.dynamicPropertyView->setModel(proxy);
  ui.dynamicPropertyView->sortByColumn(0, Qt::AscendingOrder);
  ui.dynamicPropertyView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
  ui.dynamicPropertySearchLine->setProxy(proxy);

  proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(m_methodModel);
  ui.methodView->setModel(proxy);
  ui.methodView->sortByColumn(0, Qt::AscendingOrder);
  ui.methodView->header()->setResizeMode(QHeaderView::ResizeToContents);
  ui.methodSearchLine->setProxy(proxy);
  connect(ui.methodView, SIGNAL(doubleClicked(QModelIndex)),
          SLOT(methodActivated(QModelIndex)));
  connect(ui.methodView, SIGNAL(customContextMenuRequested(QPoint)),
          SLOT(methodConextMenu(QPoint)));
  ui.methodLog->setModel(m_methodLogModel);

  proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(m_classInfoModel);
  ui.classInfoView->setModel(proxy);
  ui.classInfoView->sortByColumn(0, Qt::AscendingOrder);
  ui.classInfoView->header()->setResizeMode(QHeaderView::ResizeToContents);
  ui.classInfoSearchLine->setProxy(proxy);

  m_inboundConnectionModel->setSourceModel(Probe::instance()->connectionModel());
  ui.inboundConnectionView->setModel(m_inboundConnectionModel);
  ui.inboundConnectionView->sortByColumn(0, Qt::AscendingOrder);
  ui.inboundConnectionSearchLine->setProxy(m_inboundConnectionModel);

  m_outboundConnectionModel->setSourceModel(Probe::instance()->connectionModel());
  ui.outboundConnectionView->setModel(m_outboundConnectionModel);
  ui.outboundConnectionView->sortByColumn(0, Qt::AscendingOrder);
  ui.outboundConnectionSearchLine->setProxy(m_outboundConnectionModel);

  proxy = new KRecursiveFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(m_enumModel);
  ui.enumView->setModel(proxy);
  ui.enumView->sortByColumn(0, Qt::AscendingOrder);
  ui.enumView->header()->setResizeMode(QHeaderView::ResizeToContents);
  ui.enumSearchLine->setProxy(proxy);
}

void GammaRay::PropertyWidget::setObject(QObject *object)
{
  m_object = object;
  m_staticPropertyModel->setObject(object);
  m_dynamicPropertyModel->setObject(object);
  m_classInfoModel->setObject(object);
  m_methodModel->setObject(object);
  m_inboundConnectionModel->filterReceiver(object);
  m_outboundConnectionModel->filterSender(object);
  m_enumModel->setObject(object);

  delete m_signalMapper;
  m_signalMapper = new MultiSignalMapper(this);
  connect(m_signalMapper, SIGNAL(signalEmitted(QObject*,int)), SLOT(signalEmitted(QObject*,int)));

  m_methodLogModel->clear();
}

void PropertyWidget::methodActivated(const QModelIndex &index)
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
                      arg(sender->metaObject()->method(signalIndex).signature())));
}

void PropertyWidget::methodConextMenu(const QPoint &pos)
{
  const QModelIndex index = ui.methodView->indexAt(pos);
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

  if (contextMenu.exec(ui.methodView->viewport()->mapToGlobal(pos))) {
    methodActivated(index);
  }
}

#include "propertywidget.moc"
