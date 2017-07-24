/*
  propertytexteditor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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
    connect(ui->modeButton, SIGNAL(clicked(bool)), this, SLOT(toggleMode()));
}

PropertyTextEditorDialog::PropertyTextEditorDialog(const QByteArray &bytes, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PropertyTextEditorDialog)
    , m_bytes(bytes)
    , m_mode(NoMode)
{
    ui->setupUi(this);
    setMode(StringMode);
    connect(ui->modeButton, SIGNAL(clicked(bool)), this, SLOT(toggleMode()));
}

PropertyTextEditorDialog::~PropertyTextEditorDialog()
{
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
    , m_lineEdit(new QLineEdit)
{
    setInlineWidget(m_lineEdit, [](QWidget *widget, const QString &text){
        auto edit = qobject_cast<QLineEdit*>(widget);
        edit->setText(text);
    });
    connect(m_lineEdit, SIGNAL(editingFinished()), this, SLOT(editFinished()));
}

void PropertyTextEditor::edit()
{
    PropertyTextEditorDialog dlg(value().toString(), this);
    if (dlg.exec() == QDialog::Accepted)
        save(dlg.text());
}

void PropertyTextEditor::editFinished()
{
    save(m_lineEdit->text());
}

PropertyByteArrayEditor::PropertyByteArrayEditor(QWidget *parent)
    : PropertyExtendedEditor(parent)
{

}

void PropertyByteArrayEditor::edit()
{
    PropertyTextEditorDialog dlg(value().toByteArray(), this);
    if (dlg.exec() == QDialog::Accepted)
        save(dlg.bytes());
}

