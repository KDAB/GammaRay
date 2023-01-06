/*
  quickpaintanalyzerextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QUICKPAINTANALYZEREXTENSION_H
#define GAMMARAY_QUICKPAINTANALYZEREXTENSION_H

#include <core/propertycontrollerextension.h>

namespace GammaRay {
class PaintAnalyzer;
class PropertyController;

class QuickPaintAnalyzerExtension : public PropertyControllerExtension
{
public:
    explicit QuickPaintAnalyzerExtension(PropertyController *controller);
    ~QuickPaintAnalyzerExtension();

    bool setQObject(QObject *object) override;

private:
    PaintAnalyzer *m_paintAnalyzer;
};
}

#endif // GAMMARAY_QUICKPAINTANALYZEREXTENSION_H
