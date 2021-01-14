/*
  qquickopenglshadereffectmaterialadaptor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2017-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qquickopenglshadereffectmaterialadaptor.h"

#include <core/propertydata.h>

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    if (object().type() == ObjectInstance::Object) {
        return 2;
    }
    if (object().type() == ObjectInstance::QtVariant) {
        return 1;
    }
#endif
    return 0;
}

PropertyData QQuickOpenGLShaderEffectMaterialAdaptor::propertyData(int index) const
{
    PropertyData pd;
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)

    if (object().type() == ObjectInstance::Object) {
        auto mat = reinterpret_cast<QQuickOpenGLShaderEffectMaterial*>(object().object());
        switch (index) {
            case 0:
                pd.setName(QStringLiteral("Vertex Uniforms"));
                break;
            case 1:
                pd.setName(QStringLiteral("Fragment Uniforms"));
                break;
        }
        pd.setValue(QVariant::fromValue(mat->uniforms[index]));
        pd.setClassName(QStringLiteral("QQuickOpenGLShaderEffectMaterial"));
        return pd;
    }

    if (object().type() == ObjectInstance::QtVariant) {
        const auto ud = object().variant().value<QQuickOpenGLShaderEffectMaterial::UniformData>();
        pd.setName(ud.name);
        pd.setValue(ud.value);
        pd.setTypeName(ud.value.typeName());
        pd.setClassName(QStringLiteral("QQuickOpenGLShaderEffectMaterial::UniformData"));

        // special values are only filled in dynamically from the owning node, so we wont see those
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

        return pd;
    }
#else
    Q_UNUSED(index);
#endif

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
