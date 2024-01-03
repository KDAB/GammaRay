/*
  codecbrowserwidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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

    m_stateManager.setDefaultSizes(ui->mainSplitter, UISizeVector() << "50%"
                                                                    << "50%");
}

CodecBrowserWidget::~CodecBrowserWidget() = default;

void CodecBrowserWidget::textChanged(const QString &text)
{
    Endpoint::instance()->invokeObject(QStringLiteral(
                                           "com.kdab.GammaRay.CodecBrowser"),
                                       "textChanged",
                                       QVariantList() << text);
}
