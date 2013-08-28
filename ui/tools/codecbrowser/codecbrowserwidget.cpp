/*
  codecbrowserwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#include <deferredresizemodesetter.h>
#include <network/objectbroker.h>
#include <network/networkobject.h>

using namespace GammaRay;

CodecBrowserWidget::CodecBrowserWidget(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::CodecBrowserWidget)
{
  ui->setupUi(this);

  ui->codecList->setRootIsDecorated(false);
  ui->codecList->setModel(ObjectBroker::model("com.kdab.GammaRay.AllCodecsModel"));
  ui->codecList->setSelectionModel(ObjectBroker::selectionModel(ui->codecList->model()));
  ui->codecList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->selectedCodecs->setRootIsDecorated(false);
  ui->selectedCodecs->setModel(ObjectBroker::model("com.kdab.GammaRay.SelectedCodecsModel"));

  new DeferredResizeModeSetter(ui->codecList->header(), 0, QHeaderView::ResizeToContents);
  new DeferredResizeModeSetter(ui->selectedCodecs->header(), 0, QHeaderView::ResizeToContents);

  connect(ui->codecText, SIGNAL(textChanged(QString)), SLOT(textChanged(QString)));
}

void CodecBrowserWidget::textChanged(const QString& text)
{
  ObjectBroker::objectInternal("com.kdab.GammaRay.CodecBrowser")->emitSignal("textChanged", QVariantList() << text);
}

#include "codecbrowserwidget.moc"
