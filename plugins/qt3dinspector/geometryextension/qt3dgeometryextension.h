/*
  qt3dgeometryextension.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
