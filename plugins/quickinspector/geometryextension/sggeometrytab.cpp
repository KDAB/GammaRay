/*
  sggeometrytab.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "sggeometrytab.h"
#include "common/objectbroker.h"
#include "ui/propertywidget.h"
#include "ui_sggeometrytab.h"

#include <QSortFilterProxyModel>

using namespace GammaRay;

SGGeometryTab::SGGeometryTab(PropertyWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui_SGGeometryTab)
{
    m_ui->setupUi(this);
    const QString viewName = m_ui->tableView->objectName();
    m_ui->tableView->horizontalHeader()->setObjectName(QString::fromLatin1("%1Horizontal").arg(viewName));
    m_ui->tableView->verticalHeader()->setObjectName(QString::fromLatin1("%1Vertical").arg(viewName));

    setObjectBaseName(parent->objectBaseName());
}

SGGeometryTab::~SGGeometryTab() = default;

void SGGeometryTab::setObjectBaseName(const QString &baseName)
{
    m_vertexModel = ObjectBroker::model(baseName + '.' + "sgGeometryVertexModel");
    m_adjacencyModel = ObjectBroker::model(baseName + '.' + "sgGeometryAdjacencyModel");

    auto *proxy = new QSortFilterProxyModel(this);
    proxy->setDynamicSortFilter(true);
    proxy->setSourceModel(m_vertexModel);
    m_ui->tableView->setModel(proxy);
    auto *selectionModel = new QItemSelectionModel(proxy);
    m_ui->tableView->setSelectionModel(selectionModel);

    m_ui->wireframeWidget->setModel(m_vertexModel, m_adjacencyModel);
    m_ui->wireframeWidget->setHighlightModel(selectionModel);
}
