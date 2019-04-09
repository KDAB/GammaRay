/*
  stacktracetab.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
