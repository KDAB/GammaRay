/*
  paintanalyzertab.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
