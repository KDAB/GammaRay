/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Kevin Funk <kevin.funk@kdab.com>

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

#include "actioninspector.h"
#include "actionmodel.h"

#include <common/objectmodel.h>
#include <core/probeinterface.h>
#include <common/objectbroker.h>

#include <QtPlugin>

#include <iostream>

using namespace GammaRay;
using namespace std;

ActionInspector::ActionInspector(ProbeInterface *probe, QObject *parent)
  : QObject(parent)
{
  ObjectBroker::registerObject("com.kdab.GammaRay.ActionInspector", this);

  ActionModel *actionFilterProxy = new ActionModel(this);
  actionFilterProxy->setSourceModel(probe->objectListModel());
  probe->registerModel("com.kdab.GammaRay.ActionModel", actionFilterProxy);
}

ActionInspector::~ActionInspector()
{
}

void ActionInspector::triggerAction(int row)
{
  QAbstractItemModel *model = ObjectBroker::model("com.kdab.GammaRay.ActionModel");
  const QModelIndex index = model->index(row, 0);
  if (!index.isValid()) {
    return;
  }

  QObject *obj = index.data(ObjectModel::ObjectRole).value<QObject*>();
  QAction *action = qobject_cast<QAction*>(obj);

  if (action) {
    action->trigger();
  }
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(ActionInspectorFactory)
#endif
