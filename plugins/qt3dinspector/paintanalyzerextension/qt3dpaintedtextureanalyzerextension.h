/*
  qt3dpaintedtextureanalyzerextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2018 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QT3DPAINTEDTEXTUREANALYZEREXTENSION_H
#define GAMMARAY_QT3DPAINTEDTEXTUREANALYZEREXTENSION_H

#include <core/propertycontrollerextension.h>

namespace GammaRay {
class PaintAnalyzer;
class PropertyController;

class Qt3DPaintedTextureAnalyzerExtension : public PropertyControllerExtension
{
public:
    explicit Qt3DPaintedTextureAnalyzerExtension(PropertyController *controller);
    ~Qt3DPaintedTextureAnalyzerExtension();

    bool setQObject(QObject *object) override;

private:
    PaintAnalyzer *m_paintAnalyzer;
};

}

#endif // GAMMARAY_QT3DPAINTEDTEXTUREANALYZEREXTENSION_H
