/*
  styleelementstatetablepage.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "styleelementstatetablepage.h"
#include "ui_styleelementstatetablepage.h"
#include "abstractstyleelementstatetable.h"

#include "styleinspectorclient.h"
#include <common/objectbroker.h>

static QObject *createStyleInspectorClient(const QString & /*name*/, QObject *parent)
{
    auto styleInspectorClient = new GammaRay::StyleInspectorClient(parent);
    styleInspectorClient->init();
    return styleInspectorClient;
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

    connect(ui->widthBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            m_interface, &StyleInspectorInterface::setCellWidth);
    connect(ui->widthBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            this, &StyleElementStateTablePage::updateCellSize);
    connect(ui->heightBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
            m_interface, &StyleInspectorInterface::setCellHeight);
    connect(ui->heightBox, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
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
