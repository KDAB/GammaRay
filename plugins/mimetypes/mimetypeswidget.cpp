/*
  mimetypeswidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
