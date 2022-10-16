/*
  propertymatrixdialog.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tobias Koenig <tobias.koenig@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    case QVariant::Matrix:
        windowTitle = tr("Edit Matrix");
        break;
#endif
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
