/*
  propertyfonteditor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "propertyfonteditor.h"

#include <QFontDialog>

using namespace GammaRay;

PropertyFontEditor::PropertyFontEditor(QWidget *parent)
    : PropertyExtendedEditor(parent)
{
}

void PropertyFontEditor::showEditor(QWidget *parent)
{
    bool ok = false;
    const QFont font = QFontDialog::getFont(&ok, value().value<QFont>(), parent);
    if (ok)
        save(font);
    emit editorClosed();
}
