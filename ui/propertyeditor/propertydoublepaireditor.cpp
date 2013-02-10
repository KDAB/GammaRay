/*
  propertydoublepaireditor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "propertydoublepaireditor.h"

#include "ui_propertydoublepaireditor.h"

using namespace GammaRay;

PropertyDoublePairEditor::PropertyDoublePairEditor(QWidget *parent)
  : QWidget(parent), ui(new Ui::PropertyDoublePairEditor)
{
  ui->setupUi(this);
}

PropertyPointFEditor::PropertyPointFEditor(QWidget *parent)
  : PropertyDoublePairEditor(parent)
{
}

QPointF PropertyPointFEditor::pointF() const
{
  return QPointF(ui->xBox->value(), ui->yBox->value());
}

void PropertyPointFEditor::setPointF(const QPointF &point)
{
  ui->xBox->setValue(point.x());
  ui->yBox->setValue(point.y());
}

PropertySizeFEditor::PropertySizeFEditor(QWidget *parent) : PropertyDoublePairEditor(parent)
{
}

QSizeF PropertySizeFEditor::sizeF() const
{
  return QSizeF(ui->xBox->value(), ui->yBox->value());
}

void PropertySizeFEditor::setSizeF(const QSizeF &size)
{
  ui->xBox->setValue(size.width());
  ui->yBox->setValue(size.height());
}

#include "propertydoublepaireditor.moc"
