/*
  propertypaletteeditor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
