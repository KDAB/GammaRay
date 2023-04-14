/*
  propertymatrixdialog.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tobias Koenig <tobias.koenig@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
