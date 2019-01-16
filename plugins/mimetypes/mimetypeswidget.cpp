/*
  mimetypeswidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "mimetypeswidget.h"
#include "ui_mimetypeswidget.h"

#include <ui/searchlinecontroller.h>

#include <common/objectbroker.h>

using namespace GammaRay;

MimeTypesWidget::MimeTypesWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MimeTypesWidget)
    , m_stateManager(this)
{
    ui->setupUi(this);

    auto model = ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.MimeTypeModel"));
    ui->mimeTypeView->header()->setObjectName("mimeTypeViewHeader");
    ui->mimeTypeView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->mimeTypeView->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    ui->mimeTypeView->setModel(model);
    ui->mimeTypeView->sortByColumn(0, Qt::AscendingOrder);
    new SearchLineController(ui->searchLine, model);
}

MimeTypesWidget::~MimeTypesWidget() = default;
