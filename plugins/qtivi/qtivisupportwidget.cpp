/*
  qtivisupportwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com

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

#include "qtivisupportwidget.h"
#include "qtivipropertymodel.h"
#include "qtiviconstrainedvaluedelegate.h"

#include <common/endpoint.h>
#include <common/objectbroker.h>
#include <ui/contextmenuextension.h>

#include <QVBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QMenu>
#include <QTreeView>

using namespace GammaRay;

QtIVIWidget::QtIVIWidget(QWidget *parent)
    : QWidget(parent)
{
    setObjectName("QtIVIWidget");
    QAbstractItemModel *propertyModel
        = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.PropertyModel"));

    QVBoxLayout *vbox = new QVBoxLayout(this);

    m_objectTreeView = new QTreeView(this);
    m_objectTreeView->header()->setObjectName("objectTreeViewHeader");
    vbox->addWidget(m_objectTreeView);

    m_objectTreeView->setModel(propertyModel);
    m_objectTreeView->setSelectionModel(ObjectBroker::selectionModel(propertyModel));
    m_objectTreeView->setItemDelegateForColumn(1, new QtIviConstrainedValueDelegate(this));

    setContextMenuPolicy(Qt::CustomContextMenu);
    m_objectTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_objectTreeView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
}

void QtIVIWidget::contextMenu(QPoint pos)
{
    QModelIndex index = m_objectTreeView->indexAt(pos);
    if (!index.isValid())
        return;
    index = index.sibling(index.row(), 0);
    const ObjectId objectId = index.data(ObjectModel::ObjectIdRole).value<ObjectId>();
    if (objectId.isNull())
        return;

    QMenu menu;
    ContextMenuExtension ext(objectId);
    ext.populateMenu(&menu);
    menu.exec(m_objectTreeView->viewport()->mapToGlobal(pos));
}
