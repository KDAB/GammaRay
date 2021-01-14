/*
  enumstab.cpp

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
