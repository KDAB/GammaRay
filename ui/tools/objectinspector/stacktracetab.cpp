/*
  stacktracetab.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "stacktracetab.h"
#include "ui_stacktracetab.h"

#include <ui/contextmenuextension.h>
#include <ui/propertywidget.h>
#include <ui/propertyeditor/propertyeditordelegate.h>

#include <common/objectbroker.h>
#include <common/sourcelocation.h>

#include <QMenu>

using namespace GammaRay;

GammaRay::StackTraceTab::StackTraceTab(PropertyWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StackTraceTab)
{
    ui->setupUi(this);
    ui->stackTraceView->setItemDelegate(new PropertyEditorDelegate(ui->stackTraceView));
    ui->stackTraceView->setModel(ObjectBroker::model(parent->objectBaseName() + QStringLiteral(".stackTraceModel")));
    ui->stackTraceView->header()->setObjectName(QStringLiteral("stackTraceViewHeader"));
    connect(ui->stackTraceView, &QWidget::customContextMenuRequested, this, &StackTraceTab::contextMenuRequested);
}

StackTraceTab::~StackTraceTab() = default;

void StackTraceTab::contextMenuRequested(QPoint pos)
{
    const auto idx = ui->stackTraceView->indexAt(pos);
    if (!idx.isValid())
        return;

    const auto loc = idx.sibling(idx.row(), 1).data().value<SourceLocation>();
    if (!loc.isValid())
        return;

    QMenu contextMenu;
    ContextMenuExtension cme;
    cme.setLocation(ContextMenuExtension::ShowSource, loc);
    cme.populateMenu(&contextMenu);
    contextMenu.exec(ui->stackTraceView->viewport()->mapToGlobal(pos));
}
