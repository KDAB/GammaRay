/*
  enumstab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "enumstab.h"
#include "ui_enumstab.h"
#include "propertywidget.h"

#include <ui/searchlinecontroller.h>
#include <common/objectbroker.h>

#include "kde/krecursivefilterproxymodel.h"

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
    QSortFilterProxyModel *proxy = new KRecursiveFilterProxyModel(this);
    proxy->setDynamicSortFilter(true);
    proxy->setSourceModel(ObjectBroker::model(baseName + '.' + "enums"));
    m_ui->enumView->setModel(proxy);
    m_ui->enumView->sortByColumn(0, Qt::AscendingOrder);
    m_ui->enumView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    new SearchLineController(m_ui->enumSearchLine, proxy);
}
