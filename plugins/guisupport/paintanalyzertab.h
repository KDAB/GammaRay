/*
  paintanalyzertab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PAINTANALYZERTAB_H
#define GAMMARAY_PAINTANALYZERTAB_H

#include <ui/paintanalyzerwidget.h>

namespace GammaRay {
class PropertyWidget;

class PaintAnalyzerTab : public PaintAnalyzerWidget
{
    Q_OBJECT
public:
    explicit PaintAnalyzerTab(PropertyWidget *parent);
    ~PaintAnalyzerTab() override;
};
}

#endif // GAMMARAY_PAINTANALYZERTAB_H
