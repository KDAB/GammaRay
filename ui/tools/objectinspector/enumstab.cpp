/*
  enumstab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "enumstab.h"
#include "ui_enumstab.h"
#include "propertywidget.h"

#include <ui/searchlinecontroller.h>
#include <common/objectbroker.h>

#include <QSortFilterProxyModel>

using namespace GammaRay;

EnumsTab::EnumsTab(PropertyWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui_EnumsTab)
{
    m_ui->setupUi(this);
    m_ui->enumView->header()->setObjectName("enumViewHeader");
    setObjectBaseName(parent->objectBaseName());
}

EnumsTab::~EnumsTab() = default;

void EnumsTab::setObjectBaseName(const QString &baseName)
{
    auto *proxy = new QSortFilterProxyModel(this);
    proxy->setDynamicSortFilter(true);
    proxy->setSourceModel(ObjectBroker::model(baseName + '.' + "enums"));
    m_ui->enumView->setModel(proxy);
    m_ui->enumView->sortByColumn(0, Qt::AscendingOrder);
    m_ui->enumView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    new SearchLineController(m_ui->enumSearchLine, proxy);
}
