/*
  classinfotab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "classinfotab.h"
#include "ui_classinfotab.h"
#include "propertywidget.h"

#include <ui/searchlinecontroller.h>
#include <common/objectbroker.h>

#include <QSortFilterProxyModel>

using namespace GammaRay;

ClassInfoTab::ClassInfoTab(PropertyWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui_ClassInfoTab)
{
    m_ui->setupUi(this);
    m_ui->classInfoView->header()->setObjectName("classInfoViewHeader");
    setObjectBaseName(parent->objectBaseName());
}

ClassInfoTab::~ClassInfoTab() = default;

void ClassInfoTab::setObjectBaseName(const QString &baseName)
{
    auto *proxy = new QSortFilterProxyModel(this);
    proxy->setDynamicSortFilter(true);
    proxy->setSourceModel(ObjectBroker::model(baseName + '.' + "classInfo"));
    m_ui->classInfoView->setModel(proxy);
    m_ui->classInfoView->sortByColumn(0, Qt::AscendingOrder);
    m_ui->classInfoView->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    new SearchLineController(m_ui->classInfoSearchLine, proxy);
}
