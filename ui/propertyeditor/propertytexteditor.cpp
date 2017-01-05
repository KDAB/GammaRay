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

#include <QToolButton>

using namespace GammaRay;

PropertyTextEditorDialog::PropertyTextEditorDialog(const QString &text, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PropertyTextEditorDialog)
{
    ui->setupUi(this);
    ui->plainTextEdit->setPlainText(text);
}

PropertyTextEditorDialog::~PropertyTextEditorDialog()
{
}

QString PropertyTextEditorDialog::text() const
{
    return ui->plainTextEdit->toPlainText();
}

PropertyTextEditor::PropertyTextEditor(QWidget *parent)
    : QLineEdit(parent)
{
    QHBoxLayout *hl = new QHBoxLayout(this);
    hl->setMargin(0);
    hl->setSpacing(1);

    QToolButton *editButton = new QToolButton(this);
    editButton->setText(QStringLiteral("..."));

    QMargins margins(textMargins());
    margins.setRight(editButton->sizeHint().width() + hl->spacing());

    setFrame(false);
    setTextMargins(margins);

    hl->addStretch();
    hl->addWidget(editButton);

    connect(editButton, SIGNAL(clicked()), SLOT(edit()));
}

void PropertyTextEditor::save(const QString &text)
{
    setText(text);

    // The user already pressed Apply, don't force her/him to do again
    QKeyEvent event(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
    QApplication::sendEvent(this, &event);
}

void PropertyTextEditor::edit()
{
    PropertyTextEditorDialog dlg(text(), this);
    if (dlg.exec() == QDialog::Accepted)
        save(dlg.text());
}
