/*
  propertyintpaireditor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
    return { ui->xBox->value(), ui->yBox->value() };
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
    return { ui->xBox->value(), ui->yBox->value() };
}

void PropertySizeEditor::setSizeValue(const QSize &size)
{
    ui->xBox->setValue(size.width());
    ui->yBox->setValue(size.height());
}
