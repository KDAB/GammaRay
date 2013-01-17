/*
  palettedialog.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2013 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "palettedialog.h"

#include "ui_palettedialog.h"
#include "palettemodel.h"
#include "propertyeditor/propertyeditorfactory.h"

#include <QPushButton>
#include <QStyledItemDelegate>

using namespace GammaRay;

PaletteDialog::PaletteDialog(const QPalette &palette, QWidget *parent)
  : QDialog(parent),
    ui(new Ui::PaletteDialog),
    m_model(new PaletteModel(this))
{
  ui->setupUi(this);
  m_model->setPalette(palette);
  m_model->setEditable(true);
  ui->paletteView->setModel(m_model);

  QStyledItemDelegate *delegate =
    qobject_cast<QStyledItemDelegate*>(ui->paletteView->itemDelegate());
  if (delegate) {
    m_propertyEditorFactory.reset(new PropertyEditorFactory);
    delegate->setItemEditorFactory(m_propertyEditorFactory.data());
  }
}

PaletteDialog::~PaletteDialog()
{
  delete ui;
}

QPalette PaletteDialog::editedPalette() const
{
  return m_model->palette();
}

#include "palettedialog.moc"
