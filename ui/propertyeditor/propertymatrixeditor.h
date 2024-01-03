/*
  propertymatrixeditor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Tobias Koenig <tobias.koenig@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PROPERTYMATRIXEDITOR_H
#define GAMMARAY_PROPERTYMATRIXEDITOR_H

#include "propertyextendededitor.h"

namespace GammaRay {
class PropertyMatrixEditor : public PropertyExtendedEditor
{
    Q_OBJECT
public:
    explicit PropertyMatrixEditor(QWidget *parent = nullptr);
    void showEditor(QWidget *parent) override;
};
}

#endif // GAMMARAY_PROPERTYMATRIXEDITOR_H
