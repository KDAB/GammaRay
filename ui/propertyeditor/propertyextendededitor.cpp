/*
  propertyextendededitor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "propertyextendededitor.h"
#include "ui_propertyextendededitor.h"

#include <QColorDialog>
#include <QKeyEvent>
#include <QLabel>

using namespace GammaRay;

PropertyExtendedEditor::PropertyExtendedEditor(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PropertyExtendedEditor)
{
    ui->setupUi(this);
    setInlineEditable(false);
    setReadOnly(false);

    // TODO: make button content smaller by using a tiny icon
    connect(ui->editButton, &QAbstractButton::clicked, this, &PropertyExtendedEditor::slotEdit);
}

PropertyExtendedEditor::~PropertyExtendedEditor() = default;

QVariant PropertyExtendedEditor::value() const
{
    if (isInlineEditable())
        return ui->lineEdit->text();
    return m_value;
}

void PropertyExtendedEditor::setValue(const QVariant &value)
{
    m_value = value;
    const QString displayValue = property("displayString").toString();
    ui->lineEdit->setText(displayValue.isEmpty() ? value.toString() : displayValue);
}

void PropertyExtendedEditor::save(const QVariant &value)
{
    if (isReadOnly())
        return;

    setValue(value);

    // The user already pressed Apply, don't force her/him to do again
    QKeyEvent event(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
    QApplication::sendEvent(this, &event);
}

bool PropertyExtendedEditor::isInlineEditable() const
{
    return m_inlineEditable;
}

void PropertyExtendedEditor::setInlineEditable(bool editable)
{
    m_inlineEditable = editable;
    ui->lineEdit->setReadOnly(!isInlineEditable());

    if (editable)
        setFocusProxy(ui->lineEdit);
    else
        setFocusProxy(ui->editButton);
    ui->lineEdit->setFrame(editable);
}

bool PropertyExtendedEditor::isReadOnly() const
{
    return m_readOnly;
}

void PropertyExtendedEditor::setReadOnly(bool readOnly)
{
    m_readOnly = readOnly;
    setInlineEditable(false);
}

void PropertyExtendedEditor::slotEdit()
{
    showEditor(this);
}
