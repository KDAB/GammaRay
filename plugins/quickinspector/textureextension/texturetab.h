/*
  texturetab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_TEXTURETAB_H
#define GAMMARAY_TEXTURETAB_H

#include <QWidget>

#include <memory>

namespace GammaRay {

class PropertyWidget;
namespace Ui {
class TextureTab;
}

class TextureTab : public QWidget
{
    Q_OBJECT
public:
    explicit TextureTab(PropertyWidget *parent);
    ~TextureTab() override;

private:
    void addInfoLine(bool isProblem, const QString &newLine);

    std::unique_ptr<Ui::TextureTab> ui;
};
}

#endif // GAMMARAY_TEXTURETAB_H
