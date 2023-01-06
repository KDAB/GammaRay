/*
  propertymargineditor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Waqar Ahmed <waqar.ahmed@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

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
