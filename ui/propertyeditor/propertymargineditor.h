/*
  propertymargineditor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/
#ifndef GAMMARAY_PROPERTYMARGINEDITOR_H
#define GAMMARAY_PROPERTYMARGINEDITOR_H

#include "propertyeditor/propertyextendededitor.h"

Q_DECLARE_METATYPE(QMargins)
Q_DECLARE_METATYPE(QMarginsF)

namespace GammaRay {

class PropertyMarginsEditor : public PropertyExtendedEditor
{
    Q_OBJECT
public:
    explicit PropertyMarginsEditor(QWidget *parent = nullptr);
    void showEditor(QWidget *parent) override;
};

class PropertyMarginsFEditor : public PropertyExtendedEditor
{
    Q_OBJECT
public:
    explicit PropertyMarginsFEditor(QWidget *parent = nullptr);
    void showEditor(QWidget *parent) override;
};

} // namespace GammaRay

#endif
