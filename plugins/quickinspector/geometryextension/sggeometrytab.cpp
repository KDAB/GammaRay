/*
  sggeometrytab.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#include "sggeometrytab.h"
#include "sggeometryextensioninterface.h"
#include "common/objectbroker.h"
#include "ui/propertywidget.h"
#include "ui_sggeometrytab.h"

#include <QSortFilterProxyModel>

using namespace GammaRay;

SGGeometryTab::SGGeometryTab(PropertyWidget *parent) : QWidget(parent),
  m_ui(new Ui_SGGeometryTab),
  m_interface(0),
  m_model(0)
{
  m_ui->setupUi(this);

  setObjectBaseName(parent->objectBaseName());
}

SGGeometryTab::~SGGeometryTab()
{
}

void SGGeometryTab::setObjectBaseName(const QString &baseName)
{
  if (m_interface) {
    disconnect(m_interface, 0, m_ui->wireframeWidget, 0);
  }
  m_model = ObjectBroker::model(baseName + '.' + "sgGeometryModel");

  QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(m_model);
  m_ui->tableView->setModel(proxy);
  QItemSelectionModel *selectionModel = new QItemSelectionModel(proxy);
  m_ui->tableView->setSelectionModel(selectionModel);

  m_interface =
    ObjectBroker::object<SGGeometryExtensionInterface*>(baseName + ".sgGeometry");

  m_ui->wireframeWidget->setModel(m_model);
  m_ui->wireframeWidget->setHighlightModel(selectionModel);
  connect(m_interface, SIGNAL(geometryChanged(uint,QByteArray,int)),
          m_ui->wireframeWidget, SLOT(onGeometryChanged(uint,QByteArray,int)));
}
