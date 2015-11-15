/*
  codecbrowserwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <ui/deferredresizemodesetter.h>
#include <common/objectbroker.h>
#include <common/endpoint.h>

using namespace GammaRay;

CodecBrowserWidget::CodecBrowserWidget(QWidget *parent)
  : QWidget(parent),
    ui(new Ui::CodecBrowserWidget)
{
  ui->setupUi(this);

  ui->codecList->setRootIsDecorated(false);
  ui->codecList->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.AllCodecsModel")));
  ui->codecList->setSelectionModel(ObjectBroker::selectionModel(ui->codecList->model()));
  ui->codecList->setSelectionMode(QAbstractItemView::ExtendedSelection);
  ui->selectedCodecs->setRootIsDecorated(false);
  ui->selectedCodecs->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.SelectedCodecsModel")));

  new DeferredResizeModeSetter(ui->codecList->header(), 0, QHeaderView::ResizeToContents);
  new DeferredResizeModeSetter(ui->selectedCodecs->header(), 0, QHeaderView::ResizeToContents);

  connect(ui->codecText, SIGNAL(textChanged(QString)), SLOT(textChanged(QString)));
}

CodecBrowserWidget::~CodecBrowserWidget()
{
}

void CodecBrowserWidget::textChanged(const QString& text)
{
  Endpoint::instance()->invokeObject(QStringLiteral("com.kdab.GammaRay.CodecBrowser"), "textChanged", QVariantList() << text);
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(CodecBrowserUiFactory)
#endif
