/*
  styleelementstatetablepage.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "styleelementstatetablepage.h"
#include "ui_styleelementstatetablepage.h"
#include "abstractstyleelementstatetable.h"

#include "styleinspectorclient.h"
#include <common/objectbroker.h>

static QObject *createStyleInspectorClient(const QString & /*name*/, QObject *parent)
{
    return new GammaRay::StyleInspectorClient(parent);
}

using namespace GammaRay;

StyleElementStateTablePage::StyleElementStateTablePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StyleElementStateTablePage)
    , m_interface(nullptr)
{
    ObjectBroker::registerClientObjectFactoryCallback<StyleInspectorInterface *>(
        createStyleInspectorClient);
    m_interface = ObjectBroker::object<StyleInspectorInterface *>();

    ui->setupUi(this);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    ui->tableView->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);

    connect(ui->widthBox, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            m_interface, &StyleInspectorInterface::setCellWidth);
    connect(ui->widthBox,  static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &StyleElementStateTablePage::updateCellSize);
    connect(ui->heightBox,  static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            m_interface, &StyleInspectorInterface::setCellHeight);
    connect(ui->heightBox,  static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &StyleElementStateTablePage::updateCellSize);
    connect(ui->zoomSlider, &QAbstractSlider::valueChanged, m_interface, &StyleInspectorInterface::setCellZoom);
    connect(ui->zoomSlider, &QAbstractSlider::valueChanged, this, &StyleElementStateTablePage::updateCellSize);

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
    ui->tableView->verticalHeader()->setDefaultSectionSize(m_interface->cellSizeHint().height()
                                                           + 4);
    ui->tableView->horizontalHeader()->setDefaultSectionSize(m_interface->cellSizeHint().width()
                                                             + 4);
}
