/*
  palettedialog.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "palettedialog.h"

#include "ui_palettedialog.h"
#include "palettemodel.h"
#include "propertyeditor/propertyeditordelegate.h"

#include <QPushButton>
#include <QStyledItemDelegate>

using namespace GammaRay;

PaletteDialog::PaletteDialog(const QPalette &palette, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::PaletteDialog)
    , m_stateManager(this)
    , m_model(new PaletteModel(this))
{
    ui->setupUi(this);
    m_model->setPalette(palette);
    m_model->setEditable(true);
    ui->paletteView->header()->setObjectName("paletteViewHeader");
    ui->paletteView->setDeferredResizeMode(0, QHeaderView::Stretch);
    ui->paletteView->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    ui->paletteView->setDeferredResizeMode(2, QHeaderView::ResizeToContents);
    ui->paletteView->setDeferredResizeMode(3, QHeaderView::ResizeToContents);
    ui->paletteView->setModel(m_model);
    ui->paletteView->setItemDelegate(new PropertyEditorDelegate(this));
}

PaletteDialog::~PaletteDialog()
{
    delete ui;
}

QPalette PaletteDialog::editedPalette() const
{
    return m_model->palette();
}

void PaletteDialog::setEditable(bool editable)
{
    m_model->setEditable(editable);
    ui->buttonBox->button(QDialogButtonBox::Save)->setEnabled(editable);
}
