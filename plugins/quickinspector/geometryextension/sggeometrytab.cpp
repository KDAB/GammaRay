/*
  sggeometrytab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    proxy->setSourceModel(m_vertexModel);
    m_ui->tableView->setModel(proxy);
    auto *selectionModel = new QItemSelectionModel(proxy);
    m_ui->tableView->setSelectionModel(selectionModel);

    m_ui->wireframeWidget->setModel(m_vertexModel, m_adjacencyModel);
    m_ui->wireframeWidget->setHighlightModel(selectionModel);
}
