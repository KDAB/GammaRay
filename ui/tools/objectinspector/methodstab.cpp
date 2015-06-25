/*
  methodstab.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#include "methodstab.h"
#include "ui_methodstab.h"
#include "propertywidget.h"

#include <ui/methodinvocationdialog.h>
#include <ui/propertybinder.h>

#include "common/objectbroker.h"
#include "common/metatypedeclarations.h"
#include "common/modelroles.h"
#include "common/tools/objectinspector/methodsextensioninterface.h"

#include <QSortFilterProxyModel>
#include <QMetaMethod>
#include <QMenu>

using namespace GammaRay;

MethodsTab::MethodsTab(PropertyWidget *parent) : QWidget(parent),
  m_ui(new Ui_MethodsTab),
  m_interface(0)
{
  m_ui->setupUi(this);
  setObjectBaseName(parent->objectBaseName());
}

MethodsTab::~MethodsTab()
{
}

void MethodsTab::setObjectBaseName(const QString &baseName)
{
  m_objectBaseName = baseName;

  QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(ObjectBroker::model(baseName + '.' + "methods"));
  proxy->setSortCaseSensitivity(Qt::CaseInsensitive);
  proxy->setSortRole(ObjectMethodModelRole::MethodSignature);
  m_ui->methodView->setModel(proxy);
  m_ui->methodView->sortByColumn(0, Qt::AscendingOrder);
  m_ui->methodView->setSelectionModel(ObjectBroker::selectionModel(proxy));
  m_ui->methodView->header()->setResizeMode(QHeaderView::ResizeToContents);
  m_ui->methodSearchLine->setProxy(proxy);
  connect(m_ui->methodView, SIGNAL(doubleClicked(QModelIndex)),
          SLOT(methodActivated(QModelIndex)));
  connect(m_ui->methodView, SIGNAL(customContextMenuRequested(QPoint)),
          SLOT(methodContextMenu(QPoint)));
  m_ui->methodLog->setModel(ObjectBroker::model(baseName + '.' + "methodLog"));

  m_interface = ObjectBroker::object<MethodsExtensionInterface*>(baseName + ".methodsExtension");
  new PropertyBinder(m_interface, "hasObject", m_ui->methodLog, "visible");
}

void MethodsTab::methodActivated(const QModelIndex &index)
{
  if (!index.isValid() || !m_interface->hasObject()) {
    return;
  }
  m_interface->activateMethod();

  MethodInvocationDialog dlg(this);
  dlg.setArgumentModel(ObjectBroker::model(m_objectBaseName + '.' + "methodArguments"));
  if (dlg.exec()) {
    m_interface->invokeMethod(dlg.connectionType());
  }
}

void MethodsTab::methodContextMenu(const QPoint &pos)
{
  const QModelIndex index = m_ui->methodView->indexAt(pos);
  if (!index.isValid() || !m_interface->hasObject()) {
    return;
  }

  const QMetaMethod::MethodType methodType =
    index.data(ObjectMethodModelRole::MetaMethodType).value<QMetaMethod::MethodType>();
  QMenu contextMenu;
  QAction *invokeAction = 0, *connectToAction = 0;
  if (methodType == QMetaMethod::Slot || methodType == QMetaMethod::Method) {
    invokeAction = contextMenu.addAction(tr("Invoke"));
  } else if (methodType == QMetaMethod::Signal) {
    connectToAction = contextMenu.addAction(tr("Connect to"));
    invokeAction = contextMenu.addAction(tr("Emit"));
  } else {
    return; // Can't do any action, so don't try to show an empty context menu.
  }

  QAction *action = contextMenu.exec(m_ui->methodView->viewport()->mapToGlobal(pos));
  if (action == invokeAction) {
    methodActivated(index);
  } else if (action == connectToAction) {
    m_interface->connectToSignal();
  }
}
