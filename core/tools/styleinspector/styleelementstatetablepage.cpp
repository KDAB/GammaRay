/*
  styleelementstatetablepage.cpp

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

#include "styleelementstatetablepage.h"
#include "ui_styleelementstatetablepage.h"
#include "abstractstyleelementstatetable.h"

using namespace GammaRay;

StyleElementStateTablePage::StyleElementStateTablePage(QWidget *parent)
  : QWidget(parent), ui(new Ui::StyleElementStateTablePage)
{
  ui->setupUi(this);
  ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::ResizeToContents);
  ui->tableView->verticalHeader()->setResizeMode(QHeaderView::ResizeToContents);
}

StyleElementStateTablePage::~StyleElementStateTablePage()
{
  delete ui;
}

void StyleElementStateTablePage::setModel(AbstractStyleElementStateTable *model)
{
  ui->tableView->setModel(model);
  ui->widthBox->setValue(model->cellWidth());
  ui->heightBox->setValue(model->cellHeight());

  connect(ui->widthBox, SIGNAL(valueChanged(int)),
          model, SLOT(setCellWidth(int)));

  connect(ui->heightBox, SIGNAL(valueChanged(int)),
          model, SLOT(setCellHeight(int)));

  connect(ui->zoomSlider, SIGNAL(valueChanged(int)),
          model, SLOT(setZoomFactor(int)));
}

#include "styleelementstatetablepage.moc"
