/*
  propertymatrixeditor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tobias Koenig <tobias.koenig@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "propertymatrixeditor.h"

#include "propertymatrixdialog.h"

using namespace GammaRay;

PropertyMatrixEditor::PropertyMatrixEditor(QWidget *parent)
    : PropertyExtendedEditor(parent)
{
}

void PropertyMatrixEditor::showEditor(QWidget *parent)
{
    PropertyMatrixDialog dlg(parent);
    dlg.setMatrix(value());

    if (dlg.exec())
        save(dlg.matrix());
    emit editorClosed();
}
