/*
  mimetypes.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "mimetypes.h"
#include "mimetypesmodel.h"
#include "ui_mimetypes.h"

#include <kde/krecursivefilterproxymodel.h>

using namespace GammaRay;

MimeTypes::MimeTypes(ProbeInterface *probe, QWidget *parent)
  : QWidget(parent), ui(new Ui::MimeTypes)
{
  Q_UNUSED(probe);
  ui->setupUi(this);

  m_model = new MimeTypesModel(this);

  QSortFilterProxyModel *proxy = new KRecursiveFilterProxyModel(this);
  proxy->setDynamicSortFilter(true);
  proxy->setSourceModel(m_model);
  ui->mimeTypeView->setModel(proxy);
  ui->mimeTypeView->header()->setResizeMode(0, QHeaderView::ResizeToContents);
  ui->mimeTypeView->header()->setResizeMode(1, QHeaderView::ResizeToContents);
  ui->mimeTypeView->sortByColumn(0, Qt::AscendingOrder);
  ui->searchLine->setProxy(proxy);
}

MimeTypes::~MimeTypes()
{
}

#include "mimetypes.moc"
