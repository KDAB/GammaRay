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

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#include <private/qquickopenglshadereffectnode_p.h>

Q_DECLARE_METATYPE(QQuickOpenGLShaderEffectMaterial::UniformData)
#endif

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
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        auto mat = reinterpret_cast<QQuickOpenGLShaderEffectMaterial *>(object().object());
        switch (index) {
        case 0:
            pd.setName(QStringLiteral("Vertex Uniforms"));
            break;
        case 1:
            pd.setName(QStringLiteral("Fragment Uniforms"));
            break;
        }
        pd.setValue(QVariant::fromValue(mat->uniforms[index]));
#endif
        pd.setClassName(QStringLiteral("QQuickOpenGLShaderEffectMaterial"));
        return pd;
    }

    if (object().type() == ObjectInstance::QtVariant) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
        const auto ud = object().variant().value<QQuickOpenGLShaderEffectMaterial::UniformData>();
        pd.setName(ud.name);
        pd.setValue(ud.value);
        pd.setTypeName(ud.value.typeName());
        pd.setClassName(QStringLiteral("QQuickOpenGLShaderEffectMaterial::UniformData"));

        // special values are only filled in dynamically from the owning node, so we won't see those
        switch (ud.specialType) {
        case QQuickOpenGLShaderEffectMaterial::UniformData::Opacity:
            pd.setTypeName(QStringLiteral("double"));
            pd.setValue(QStringLiteral("<see node>"));
            break;
        case QQuickOpenGLShaderEffectMaterial::UniformData::Matrix:
            pd.setTypeName(QStringLiteral("QMatrix4x4"));
            pd.setValue(QStringLiteral("<see node>"));
            break;
        default:
            break;
        }
#endif

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
