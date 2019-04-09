/*
  bindingtab.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Authors: Volker Krause <volker.krause@kdab.com>
           Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

BindingTab::BindingTab(PropertyWidget* parent)
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
