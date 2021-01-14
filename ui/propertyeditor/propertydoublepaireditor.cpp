/*
  propertydoublepaireditor.cpp

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

#include "propertydoublepaireditor.h"

#include "ui_propertydoublepaireditor.h"

using namespace GammaRay;

PropertyDoublePairEditor::PropertyDoublePairEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PropertyDoublePairEditor)
{
    ui->setupUi(this);
}

PropertyDoublePairEditor::~PropertyDoublePairEditor() = default;

PropertyPointFEditor::PropertyPointFEditor(QWidget *parent)
    : PropertyDoublePairEditor(parent)
{
}

QPointF PropertyPointFEditor::pointF() const
{
    return {ui->xBox->value(), ui->yBox->value()};
}

void PropertyPointFEditor::setPointF(const QPointF &point)
{
    ui->xBox->setValue(point.x());
    ui->yBox->setValue(point.y());
}

PropertySizeFEditor::PropertySizeFEditor(QWidget *parent)
    : PropertyDoublePairEditor(parent)
{
}

QSizeF PropertySizeFEditor::sizeF() const
{
    return {ui->xBox->value(), ui->yBox->value()};
}

void PropertySizeFEditor::setSizeF(const QSizeF &size)
{
    ui->xBox->setValue(size.width());
    ui->yBox->setValue(size.height());
}
