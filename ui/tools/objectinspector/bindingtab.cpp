/*
  bindingtab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

// Own
#include "bindingtab.h"
#include "ui_bindingtab.h"

#include <common/objectbroker.h>
#include <common/objectmodel.h>
#include <common/sourcelocation.h>

#include <ui/propertywidget.h>
#include <ui/contextmenuextension.h>

// Qt
#include <QMenu>

using namespace GammaRay;

BindingTab::BindingTab(PropertyWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BindingTab)
{
    ui->setupUi(this);
    ui->bindingView->setObjectName("bindingView");
    ui->bindingView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->bindingView->header()->setObjectName("bindingViewHeader");
    ui->bindingView->setModel(ObjectBroker::model(parent->objectBaseName() + QStringLiteral(".bindingModel")));
    connect(ui->bindingView, &QWidget::customContextMenuRequested,
            this, &BindingTab::bindingContextMenu);
}

BindingTab::~BindingTab() = default;

void GammaRay::BindingTab::bindingContextMenu(const QPoint &pos)
{
    const auto index = ui->bindingView->indexAt(pos);
    if (!index.isValid())
        return;

    QMenu menu;
    ContextMenuExtension ext;
    ext.setLocation(ContextMenuExtension::ShowSource,
                    index.data(ObjectModel::DeclarationLocationRole).value<SourceLocation>());
    ext.populateMenu(&menu);

    menu.exec(ui->bindingView->viewport()->mapToGlobal(pos));
}
