/*
  qquickopenglshadereffectmaterialadaptor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_QQUICKOPENGLSHADEREFFECTMATERIALADAPTOR_H
#define GAMMARAY_QQUICKOPENGLSHADEREFFECTMATERIALADAPTOR_H

#include <core/propertyadaptor.h>
#include <core/propertyadaptorfactory.h>

namespace GammaRay {

class QQuickOpenGLShaderEffectMaterialAdaptor : public PropertyAdaptor
{
    Q_OBJECT
public:
    explicit QQuickOpenGLShaderEffectMaterialAdaptor(QObject *parent = nullptr);
    ~QQuickOpenGLShaderEffectMaterialAdaptor() override;

    int count() const override;
    PropertyData propertyData(int index) const override;
};

class QQuickOpenGLShaderEffectMaterialAdaptorFactory : public AbstractPropertyAdaptorFactory
{
public:
    PropertyAdaptor *create(const ObjectInstance &oi, QObject *parent = nullptr) const override;
    static QQuickOpenGLShaderEffectMaterialAdaptorFactory *instance();

private:
    static QQuickOpenGLShaderEffectMaterialAdaptorFactory *s_instance;
};

}

#endif // GAMMARAY_QQUICKOPENGLSHADEREFFECTMATERIALADAPTOR_H
