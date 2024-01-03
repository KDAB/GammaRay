/*
  propertypaletteeditor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "propertypaletteeditor.h"
#include "palettedialog.h"

using namespace GammaRay;

PropertyPaletteEditor::PropertyPaletteEditor(QWidget *parent)
    : PropertyExtendedEditor(parent)
{
}

void PropertyPaletteEditor::showEditor(QWidget *parent)
{
    PaletteDialog dlg(value().value<QPalette>(), parent);
    dlg.setEditable(!isReadOnly());
    if (dlg.exec() == QDialog::Accepted)
        save(dlg.editedPalette());
    emit editorClosed();
}
