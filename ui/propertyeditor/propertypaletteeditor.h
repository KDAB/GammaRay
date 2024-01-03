/*
  propertypaletteeditor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2012 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROPERTYPALETTEEDITOR_H
#define GAMMARAY_PROPERTYPALETTEEDITOR_H

#include "propertyextendededitor.h"

namespace GammaRay {
class PropertyPaletteEditor : public PropertyExtendedEditor
{
    Q_OBJECT
public:
    explicit PropertyPaletteEditor(QWidget *parent = nullptr);
    void showEditor(QWidget *parent) override;
};
}

#endif // GAMMARAY_PROPERTYPALETTEEDITOR_H
