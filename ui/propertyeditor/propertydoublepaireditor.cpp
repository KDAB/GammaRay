/*
  propertydoublepaireditor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    return { ui->xBox->value(), ui->yBox->value() };
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
    return { ui->xBox->value(), ui->yBox->value() };
}

void PropertySizeFEditor::setSizeF(const QSizeF &size)
{
    ui->xBox->setValue(size.width());
    ui->yBox->setValue(size.height());
}
