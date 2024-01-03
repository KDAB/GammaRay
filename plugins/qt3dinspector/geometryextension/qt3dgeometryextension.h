/*
  qt3dgeometryextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QT3DGEOMETRYEXTENSION_H
#define GAMMARAY_QT3DGEOMETRYEXTENSION_H

#include "qt3dgeometryextensioninterface.h"

#include <core/propertycontrollerextension.h>

QT_BEGIN_NAMESPACE
namespace Qt3DRender {
class QGeometryRenderer;
}
QT_END_NAMESPACE

namespace GammaRay {
class Qt3DGeometryExtension : public Qt3DGeometryExtensionInterface,
                              public PropertyControllerExtension
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::Qt3DGeometryExtensionInterface)
public:
    explicit Qt3DGeometryExtension(PropertyController *controller);
    ~Qt3DGeometryExtension();

    bool setQObject(QObject *object) override;

private:
    void updateGeometryData();

    Qt3DRender::QGeometryRenderer *m_geometry;
};
}

#endif // GAMMARAY_QT3DGEOMETRYEXTENSION_H
