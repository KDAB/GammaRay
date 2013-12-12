/*
  metaobjectbrowser.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "metaobjectbrowser.h"
#include "metaobjecttreemodel.h"
#include "probe.h"
#include "propertycontroller.h"

#include <common/objectbroker.h>

#include <QDebug>
#include <QItemSelectionModel>

using namespace GammaRay;

MetaObjectBrowser::MetaObjectBrowser(ProbeInterface *probe, QObject *parent)
  : QObject(parent), m_propertyController(new PropertyController("com.kdab.GammaRay.MetaObjectBrowser", this))
{
  Q_UNUSED(probe);
  QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(Probe::instance()->metaObjectModel());

  connect(selectionModel,SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(objectSelected(QItemSelection)));

  m_propertyController->setMetaObject(0); // init
}

void MetaObjectBrowser::objectSelected(const QItemSelection &selection)
{
  QModelIndex index;
  if (selection.size() == 1)
    index = selection.first().topLeft();

  if (index.isValid()) {
    const QMetaObject *metaObject =
      index.data(MetaObjectTreeModel::MetaObjectRole).value<const QMetaObject*>();
    m_propertyController->setMetaObject(metaObject);
  } else {
    m_propertyController->setMetaObject(0);
  }
}

