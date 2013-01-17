/*
  metatypebrowser.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "metatypebrowser.h"
#include "ui_metatypebrowser.h"

#include "metatypesmodel.h"

#include <QSortFilterProxyModel>

using namespace GammaRay;

MetaTypeBrowser::MetaTypeBrowser(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent),
    ui(new Ui::MetaTypeBrowser)
{
  Q_UNUSED(probe);
  ui->setupUi(this);

  MetaTypesModel *mtm = new MetaTypesModel(this);
  QSortFilterProxyModel *proxy = new QSortFilterProxyModel(this);
  proxy->setSourceModel(mtm);
  ui->metaTypeView->setModel(proxy);
  ui->metaTypeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
  ui->metaTypeSearchLine->setProxy(proxy);
  ui->metaTypeView->header()->setSortIndicator(1, Qt::AscendingOrder); // sort by type id
}

#include "metatypebrowser.moc"
