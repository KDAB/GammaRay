/*
  propertytexteditor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "propertytexteditor.h"

#include "ui_propertytexteditor.h"

using namespace GammaRay;

PropertyTextEditorDialog::PropertyTextEditorDialog(const QString &text, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PropertyTextEditorDialog)
    , m_bytes(text.toUtf8())
    , m_mode(NoMode)
{
    ui->setupUi(this);
    setMode(StringMode);
    connect(ui->modeButton, &QAbstractButton::clicked, this, &PropertyTextEditorDialog::toggleMode);
}

PropertyTextEditorDialog::PropertyTextEditorDialog(const QByteArray &bytes, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PropertyTextEditorDialog)
    , m_bytes(bytes)
    , m_mode(NoMode)
{
    ui->setupUi(this);
    setMode(StringMode);
    connect(ui->modeButton, &QAbstractButton::clicked, this, &PropertyTextEditorDialog::toggleMode);
}

PropertyTextEditorDialog::~PropertyTextEditorDialog() = default;

void PropertyTextEditorDialog::setReadOnly(bool readOnly)
{
    ui->plainTextEdit->setReadOnly(readOnly);
}

QString PropertyTextEditorDialog::text() const
{
    if (m_mode == StringMode)
        return ui->plainTextEdit->toPlainText();
    else
        return QString::fromUtf8(bytes());
}

QByteArray PropertyTextEditorDialog::bytes() const
{
    if (m_mode == StringMode)
        return ui->plainTextEdit->toPlainText().toUtf8();
    else
        return QByteArray::fromHex(ui->plainTextEdit->toPlainText().toUtf8());
}

void PropertyTextEditorDialog::setMode(PropertyTextEditorDialog::Mode mode)
{
    if (m_mode != mode) {
        m_mode = mode;
        if (m_mode == StringMode) {
            ui->modeButton->setText(tr("Switch to Hex mode"));
            ui->plainTextEdit->setPlainText(QString::fromUtf8(m_bytes));
        } else {
            ui->modeButton->setText(tr("Switch to String mode"));
            ui->plainTextEdit->setPlainText(m_bytes.toHex());
        }
    }
}

void PropertyTextEditorDialog::toggleMode()
{
    setMode(m_mode == StringMode ? HexMode : StringMode);
}

PropertyTextEditor::PropertyTextEditor(QWidget *parent)
    : PropertyExtendedEditor(parent)
{
    setInlineEditable(true);
}

void PropertyTextEditor::showEditor(QWidget *parent)
{
    PropertyTextEditorDialog dlg(value().toString(), parent);
    dlg.setReadOnly(isReadOnly());
    if (dlg.exec() == QDialog::Accepted)
        save(dlg.text());
    emit editorClosed();
}

PropertyByteArrayEditor::PropertyByteArrayEditor(QWidget *parent)
    : PropertyExtendedEditor(parent)
{
}

void PropertyByteArrayEditor::showEditor(QWidget *parent)
{
    PropertyTextEditorDialog dlg(value().toByteArray(), parent);
    dlg.setReadOnly(isReadOnly());
    if (dlg.exec() == QDialog::Accepted)
        save(dlg.bytes());
    emit editorClosed();
}
