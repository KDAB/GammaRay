/*
  propertymatrixdialog.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Tobias Koenig <tobias.koenig@kdab.com>

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

#include "propertymatrixdialog.h"
#include "ui_propertymatrixdialog.h"

#include "propertymatrixmodel.h"

using namespace GammaRay;

PropertyMatrixDialog::PropertyMatrixDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PropertyMatrixDialog)
    , m_model(new PropertyMatrixModel(this))
{
    ui->setupUi(this);

    ui->tableView->setModel(m_model);
}

PropertyMatrixDialog::~PropertyMatrixDialog()
{
    delete ui;
}

void PropertyMatrixDialog::setMatrix(const QVariant &matrix)
{
    m_model->setMatrix(matrix);

    QString windowTitle = tr("Edit Matrix");
    switch (matrix.type()) {
    case QVariant::Matrix:
        windowTitle = tr("Edit Matrix");
        break;
    case QVariant::Matrix4x4:
        windowTitle = tr("Edit 4x4 Matrix");
        break;
    case QVariant::Transform:
        windowTitle = tr("Edit Transform");
        break;
    case QVariant::Vector2D:
        windowTitle = tr("Edit 2D Vector");
        break;
    case QVariant::Vector3D:
        windowTitle = tr("Edit 3D Vector");
        break;
    case QVariant::Vector4D:
        windowTitle = tr("Edit 4D Vector");
        break;
    case QVariant::Quaternion:
        windowTitle = tr("Edit Quaternion");
        break;
    default:
        windowTitle = tr("Edit Unsupported Type");
        break;
    }

    setWindowTitle(windowTitle);
}

QVariant PropertyMatrixDialog::matrix() const
{
    return m_model->matrix();
}
