/*
  gridsettingswidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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
