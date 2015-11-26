/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "actioninspectorwidget.h"
#include "actionmodel.h" // for column enum only

#include <ui/searchlinecontroller.h>
#include <common/objectbroker.h>
#include <common/endpoint.h>

#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QTreeView>

using namespace GammaRay;

ActionInspectorWidget::ActionInspectorWidget(QWidget *parent)
  : QWidget(parent)
{
  QAbstractItemModel *actionModel = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.ActionModel"));

  QVBoxLayout *vbox = new QVBoxLayout(this);
  auto actionSearchLine = new QLineEdit(this);
  new SearchLineController(actionSearchLine, actionModel);
  vbox->addWidget(actionSearchLine);

  QTreeView *objectTreeView = new QTreeView(this);
  objectTreeView->setModel(actionModel);
  objectTreeView->setSortingEnabled(true);
  objectTreeView->sortByColumn(ActionModel::ShortcutsPropColumn);
  objectTreeView->setRootIsDecorated(false);
  vbox->addWidget(objectTreeView);
  connect(objectTreeView, SIGNAL(doubleClicked(QModelIndex)), SLOT(triggerAction(QModelIndex)));
}

ActionInspectorWidget::~ActionInspectorWidget()
{
}

void ActionInspectorWidget::triggerAction(const QModelIndex &index)
{
  if (!index.isValid()) {
    return;
  }

  Endpoint::instance()->invokeObject(QStringLiteral("com.kdab.GammaRay.ActionInspector"), "triggerAction",
                                     QVariantList() << index.row());
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(ActionInspectorUiFactory)
#endif
