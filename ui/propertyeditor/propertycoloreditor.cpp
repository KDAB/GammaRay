/*
  propertycoloreditor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "propertycoloreditor.h"

#include <QColorDialog>

using namespace GammaRay;

PropertyColorEditor::PropertyColorEditor(QWidget *parent)
    : PropertyExtendedEditor(parent)
{
}

void PropertyColorEditor::showEditor(QWidget *parent)
{
    const QColor color = QColorDialog::getColor(value().value<QColor>(), parent,
                                                QString(), QColorDialog::ShowAlphaChannel);
    if (color.isValid())
        save(QVariant::fromValue(color));
    emit editorClosed();
}
