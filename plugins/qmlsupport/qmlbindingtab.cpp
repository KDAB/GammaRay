/*
  qmlbindingtab.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "qmlbindingtab.h"
#include "ui_qmlbindingtab.h"

#include <ui/propertywidget.h>
#include <ui/contextmenuextension.h>
#include <common/objectbroker.h>
#include <QMenu>
#include <common/objectmodel.h>

using namespace GammaRay;

QmlBindingTab::QmlBindingTab(PropertyWidget* parent)
    : QWidget(parent)
    , ui(new Ui::QmlBindingTab)
{
    ui->setupUi(this);
    ui->bindingView->setModel(ObjectBroker::model(parent->objectBaseName() + QStringLiteral(".qmlBindingModel")));
    connect(ui->bindingView, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(bindingContextMenu(QPoint)));
}

QmlBindingTab::~QmlBindingTab()
{
}

void GammaRay::QmlBindingTab::bindingContextMenu(QPoint pos)
{
    qDebug() << "######################Huibu!";
    const auto index = ui->bindingView->indexAt(pos);
    if (!index.isValid())
        return;
    qDebug() << "indexIsValid";

    QMenu menu;
    ContextMenuExtension ext;
    ext.setLocation(ContextMenuExtension::ShowSource,
                    index.data(ObjectModel::DeclarationLocationRole).value<SourceLocation>());
    ext.populateMenu(&menu);
    qDebug() << index.data(ObjectModel::DeclarationLocationRole).value<SourceLocation>().displayString();

    menu.addAction("Foo");

    menu.exec(ui->bindingView->viewport()->mapToGlobal(pos));
}
