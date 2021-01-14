/*
  codecbrowserwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include "codecbrowserwidget.h"
#include "ui_codecbrowserwidget.h"

#include <common/objectbroker.h>
#include <common/endpoint.h>

using namespace GammaRay;

CodecBrowserWidget::CodecBrowserWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::CodecBrowserWidget)
    , m_stateManager(this)
{
    ui->setupUi(this);

    ui->codecList->header()->setObjectName("codecListHeader");
    ui->codecList->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->codecList->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.AllCodecsModel")));
    ui->codecList->setSelectionModel(ObjectBroker::selectionModel(ui->codecList->model()));

    ui->selectedCodecs->header()->setObjectName("selectedCodecsHeader");
    ui->selectedCodecs->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->selectedCodecs->setModel(ObjectBroker::model(QStringLiteral(
                                                         "com.kdab.GammaRay.SelectedCodecsModel")));

    connect(ui->codecText, &QLineEdit::textChanged, this, &CodecBrowserWidget::textChanged);

    m_stateManager.setDefaultSizes(ui->mainSplitter, UISizeVector() << "50%" << "50%");
}

CodecBrowserWidget::~CodecBrowserWidget() = default;

void CodecBrowserWidget::textChanged(const QString &text)
{
    Endpoint::instance()->invokeObject(QStringLiteral(
                                           "com.kdab.GammaRay.CodecBrowser"), "textChanged",
                                       QVariantList() << text);
}
