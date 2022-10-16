/*
  gridsettingswidget.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "gridsettingswidget.h"
#include "ui_gridsettingswidget.h"
#include "quickscreengrabber.h"

namespace GammaRay {

GridSettingsWidget::GridSettingsWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::GridSettingsWidget)
{
    ui->setupUi(this);

    connect(ui->gbEnabled, &QGroupBox::clicked, this, &GridSettingsWidget::enabledChanged);
    connect(ui->sbXOffset, &QAbstractSpinBox::editingFinished, this, &GridSettingsWidget::offsetUserChanged);
    connect(ui->sbYOffset, &QAbstractSpinBox::editingFinished, this, &GridSettingsWidget::offsetUserChanged);
    connect(ui->sbCellWidth, &QAbstractSpinBox::editingFinished, this, &GridSettingsWidget::cellSizeUserChanged);
    connect(ui->sbCellHeight, &QAbstractSpinBox::editingFinished, this, &GridSettingsWidget::cellSizeUserChanged);
}

GridSettingsWidget::~GridSettingsWidget()
{
    delete ui;
}

void GridSettingsWidget::setOverlaySettings(const QuickDecorationsSettings &settings)
{
    ui->gbEnabled->setChecked(settings.gridEnabled);
    ui->sbXOffset->setValue(settings.gridOffset.x());
    ui->sbYOffset->setValue(settings.gridOffset.y());
    ui->sbCellWidth->setValue(settings.gridCellSize.width());
    ui->sbCellHeight->setValue(settings.gridCellSize.height());
}

void GridSettingsWidget::offsetUserChanged()
{
    emit offsetChanged(QPoint(ui->sbXOffset->value(), ui->sbYOffset->value()));
}

void GridSettingsWidget::cellSizeUserChanged()
{
    emit cellSizeChanged(QSize(ui->sbCellWidth->value(), ui->sbCellHeight->value()));
}

}
