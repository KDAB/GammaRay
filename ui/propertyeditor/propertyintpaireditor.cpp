/*
  propertyintpaireditor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "propertyintpaireditor.h"
#include "ui_propertyintpaireditor.h"

#include <QDebug>

#include <limits>

using namespace GammaRay;

PropertyIntPairEditor::PropertyIntPairEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PropertyIntPairEditor)
{
    ui->setupUi(this);
    ui->xBox->setMinimum(std::numeric_limits<int>::min());
    ui->xBox->setMaximum(std::numeric_limits<int>::max());
    ui->yBox->setMinimum(std::numeric_limits<int>::min());
    ui->yBox->setMaximum(std::numeric_limits<int>::max());
}

PropertyIntPairEditor::~PropertyIntPairEditor() = default;

PropertyPointEditor::PropertyPointEditor(QWidget *parent)
    : PropertyIntPairEditor(parent)
{
}

QPoint PropertyPointEditor::point() const
{
    return {ui->xBox->value(), ui->yBox->value()};
}

void PropertyPointEditor::setPoint(const QPoint &point)
{
    ui->xBox->setValue(point.x());
    ui->yBox->setValue(point.y());
}

PropertySizeEditor::PropertySizeEditor(QWidget *parent)
    : PropertyIntPairEditor(parent)
{
}

QSize PropertySizeEditor::sizeValue() const
{
    return {ui->xBox->value(), ui->yBox->value()};
}

void PropertySizeEditor::setSizeValue(const QSize &size)
{
    ui->xBox->setValue(size.width());
    ui->yBox->setValue(size.height());
}
