/*
  propertyrecteditor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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

void PropertyRectEditor::showEditor(QWidget *parent)
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

void PropertyRectFEditor::showEditor(QWidget *parent)
{
    PropertyRectEditorDialog dlg(value().toRectF(), parent);
    if (dlg.exec() == QDialog::Accepted)
        save(dlg.rectF());
    emit editorClosed();
}
