/*
  propertymatrixdialog.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
