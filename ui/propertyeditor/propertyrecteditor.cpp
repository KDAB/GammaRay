/*
  propertyrecteditor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Hannah von Reth <hannah.vonreth@kdab.com>

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

#include "propertyrecteditor.h"
#include "ui_propertyrecteditor.h"

using namespace GammaRay;

PropertyRectEditorDialog::PropertyRectEditorDialog(const QRectF &rect, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PropertyRectEditorDialog)
{
    ui->setupUi(this);

    ui->pointFWidget->setPointF(rect.topLeft());
    ui->sizeFWidget->setSizeF(rect.size());
    ui->stackedWidget->setCurrentWidget(ui->floatPage);
}

PropertyRectEditorDialog::PropertyRectEditorDialog(const QRect &rect, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PropertyRectEditorDialog)
{
    ui->setupUi(this);

    ui->pointWidget->setPoint(rect.topLeft());
    ui->sizeWidget->setSizeValue(rect.size());
    ui->stackedWidget->setCurrentWidget(ui->intPage);
}

PropertyRectEditorDialog::~PropertyRectEditorDialog()
{
    delete ui;
}

QRectF PropertyRectEditorDialog::rectF() const
{
    if (ui->stackedWidget->currentWidget() == ui->intPage) {
        return QRectF(ui->pointWidget->point(), ui->sizeWidget->sizeValue());
    } else {
        return QRectF(ui->pointFWidget->pointF(), ui->sizeFWidget->sizeF());
    }
}

PropertyRectEditor::PropertyRectEditor(QWidget *parent)
    : PropertyExtendedEditor(parent)
{
}

void PropertyRectEditor::showEditor(QWidget* parent)
{
    PropertyRectEditorDialog dlg(value().toRect(), parent);
    if (dlg.exec() == QDialog::Accepted)
        save(dlg.rectF().toRect());
    emit editorClosed();
}

PropertyRectFEditor::PropertyRectFEditor(QWidget *parent)
    : PropertyExtendedEditor(parent)
{
}

void PropertyRectFEditor::showEditor(QWidget* parent)
{
    PropertyRectEditorDialog dlg(value().toRectF(), parent);
    if (dlg.exec() == QDialog::Accepted)
        save(dlg.rectF());
    emit editorClosed();
}
