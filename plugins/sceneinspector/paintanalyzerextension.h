/*
  paintanalyzerextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PAINTANALYZEREXTENSION_H
#define GAMMARAY_PAINTANALYZEREXTENSION_H

#include <core/propertycontrollerextension.h>

QT_BEGIN_NAMESPACE
class QGraphicsItem;
QT_END_NAMESPACE

namespace GammaRay {
class PaintAnalyzer;
class PropertyController;

class PaintAnalyzerExtension : public PropertyControllerExtension
{
public:
    explicit PaintAnalyzerExtension(PropertyController *controller);
    ~PaintAnalyzerExtension();

    bool setQObject(QObject *object) override;
    bool setObject(void *object, const QString &typeName) override;

private:
    bool analyzePainting(QGraphicsItem *item);
    PaintAnalyzer *m_paintAnalyzer;
};
}

#endif // GAMMARAY_PAINTANALYZEREXTENSION_H
