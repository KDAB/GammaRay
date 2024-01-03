/*
  propertymatrixdialog.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tobias Koenig <tobias.koenig@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROPERTYMATRIXDIALOG_H
#define GAMMARAY_PROPERTYMATRIXDIALOG_H

#include <QDialog>

namespace GammaRay {
namespace Ui {
class PropertyMatrixDialog;
}

class PropertyMatrixModel;

class PropertyMatrixDialog : public QDialog
{
    Q_OBJECT
public:
    explicit PropertyMatrixDialog(QWidget *parent = nullptr);
    ~PropertyMatrixDialog() override;

    void setMatrix(const QVariant &matrix);
    QVariant matrix() const;

private:
    Ui::PropertyMatrixDialog *ui;

    PropertyMatrixModel *m_model;
};
}

#endif // GAMMARAY_PROPERTYMATRIXDIALOG_H
