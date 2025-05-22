/*
  qquickopenglshadereffectmaterialadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "qquickopenglshadereffectmaterialadaptor.h"

#include <core/propertydata.h>

#include <QDebug>

using namespace GammaRay;

QQuickOpenGLShaderEffectMaterialAdaptor::QQuickOpenGLShaderEffectMaterialAdaptor(QObject *parent)
    : PropertyAdaptor(parent)
{
}

QQuickOpenGLShaderEffectMaterialAdaptor::~QQuickOpenGLShaderEffectMaterialAdaptor() = default;

int QQuickOpenGLShaderEffectMaterialAdaptor::count() const
{
    if (object().type() == ObjectInstance::Object) {
        return 2;
    }
    if (object().type() == ObjectInstance::QtVariant) {
        return 1;
    }
    return 0;
}

PropertyData QQuickOpenGLShaderEffectMaterialAdaptor::propertyData(int index) const
{
    Q_UNUSED(index)
    PropertyData pd;

    if (object().type() == ObjectInstance::Object) {
        pd.setClassName(QStringLiteral("QQuickOpenGLShaderEffectMaterial"));
        return pd;
    }

    if (object().type() == ObjectInstance::QtVariant) {
        return pd;
    }

    return pd;
}

QQuickOpenGLShaderEffectMaterialAdaptorFactory *QQuickOpenGLShaderEffectMaterialAdaptorFactory::s_instance = nullptr;

PropertyAdaptor *QQuickOpenGLShaderEffectMaterialAdaptorFactory::create(const ObjectInstance &oi, QObject *parent) const
{
    if (oi.type() == ObjectInstance::Object && oi.typeName() == "QQuickOpenGLShaderEffectMaterial" && oi.object())
        return new QQuickOpenGLShaderEffectMaterialAdaptor(parent);
    if (oi.type() == ObjectInstance::QtVariant && oi.typeName() == "QQuickOpenGLShaderEffectMaterial::UniformData" && oi.variant().isValid())
        return new QQuickOpenGLShaderEffectMaterialAdaptor(parent);
    return nullptr;
}

QQuickOpenGLShaderEffectMaterialAdaptorFactory *QQuickOpenGLShaderEffectMaterialAdaptorFactory::instance()
{
    if (!s_instance)
        s_instance = new QQuickOpenGLShaderEffectMaterialAdaptorFactory;
    return s_instance;
}
