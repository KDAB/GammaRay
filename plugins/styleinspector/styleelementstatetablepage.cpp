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

#include "styleinspectorclient.h"
#include <common/objectbroker.h>

static QObject* createStyleInspectorClient(const QString & /*name*/, QObject *parent)
{
  return new GammaRay::StyleInspectorClient(parent);
}

using namespace GammaRay;

StyleElementStateTablePage::StyleElementStateTablePage(QWidget *parent)
  : QWidget(parent)
  , ui(new Ui::StyleElementStateTablePage)
  , m_interface(0)
{
  ObjectBroker::registerClientObjectFactoryCallback<StyleInspectorInterface*>(createStyleInspectorClient);
  m_interface = ObjectBroker::object<StyleInspectorInterface*>();

  ui->setupUi(this);
  ui->tableView->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
  ui->tableView->verticalHeader()->setResizeMode(QHeaderView::Fixed);

  connect(ui->widthBox, SIGNAL(valueChanged(int)), m_interface, SLOT(setCellWidth(int)));
  connect(ui->widthBox, SIGNAL(valueChanged(int)), SLOT(updateCellSize()));
  connect(ui->heightBox, SIGNAL(valueChanged(int)), m_interface, SLOT(setCellWidth(int)));
  connect(ui->heightBox, SIGNAL(valueChanged(int)), SLOT(updateCellSize()));
  connect(ui->zoomSlider, SIGNAL(valueChanged(int)), m_interface, SLOT(setCellZoom(int)));
  connect(ui->zoomSlider, SIGNAL(valueChanged(int)), SLOT(updateCellSize()));

  updateCellSize();
}

void StyleElementStateTablePage::showEvent(QShowEvent *show)
{
  ui->widthBox->setValue(m_interface->cellWidth());
  ui->heightBox->setValue(m_interface->cellHeight());
  ui->zoomSlider->setValue(m_interface->cellZoom());
  QWidget::showEvent(show);
}

StyleElementStateTablePage::~StyleElementStateTablePage()
{
  delete ui;
}

void StyleElementStateTablePage::setModel(QAbstractItemModel *model)
{
  ui->tableView->setModel(model);
}

void StyleElementStateTablePage::updateCellSize()
{
  ui->tableView->verticalHeader()->setDefaultSectionSize(m_interface->cellSizeHint().height() + 4);
  ui->tableView->horizontalHeader()->setDefaultSectionSize(m_interface->cellSizeHint().width() + 4);
}
