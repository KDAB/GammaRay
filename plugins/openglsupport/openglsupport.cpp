/*
  openglsupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "openglsupport.h"

#include <core/enumrepositoryserver.h>
#include <core/enumutil.h>
#include <core/metaenum.h>
#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/varianthandler.h>

#include <QOpenGLShader>
#include <QOpenGLShaderProgram>

#ifndef QT_NO_OPENGL
Q_DECLARE_METATYPE(QOpenGLShader::ShaderType)
#endif

using namespace GammaRay;

OpenGLSupport::OpenGLSupport(Probe *probe, QObject *parent)
    : QObject(parent)
{
    Q_UNUSED(probe)
    registerMetaTypes();
    registerVariantHandler();
}

void OpenGLSupport::registerMetaTypes()
{
    MetaObject *mo;

#ifndef QT_NO_OPENGL
    MO_ADD_METAOBJECT1(QOpenGLShader, QObject);
    MO_ADD_PROPERTY_RO(QOpenGLShader, isCompiled);
    MO_ADD_PROPERTY_RO(QOpenGLShader, log);
    MO_ADD_PROPERTY_RO(QOpenGLShader, shaderId);
    MO_ADD_PROPERTY_RO(QOpenGLShader, shaderType);
    MO_ADD_PROPERTY_RO(QOpenGLShader, sourceCode);

    MO_ADD_METAOBJECT1(QOpenGLShaderProgram, QObject);
    MO_ADD_PROPERTY_RO(QOpenGLShaderProgram, isLinked);
    MO_ADD_PROPERTY_RO(QOpenGLShaderProgram, log);
// FIXME calling this asserts in debug builds of some newer Qt versions
// MO_ADD_PROPERTY_RO(QOpenGLShaderProgram, maxGeometryOutputVertices);
    MO_ADD_PROPERTY(QOpenGLShaderProgram, patchVertexCount, setPatchVertexCount);
    MO_ADD_PROPERTY_RO(QOpenGLShaderProgram, programId);
#endif // QT_NO_OPENGL
}

#ifndef QT_NO_OPENGL
static QString shaderTypeToString(const QOpenGLShader::ShaderType type)
{
    QStringList types;
#define ST(t) if (type & QOpenGLShader::t) types.push_back(QStringLiteral(#t));
    ST(Vertex)
    ST(Fragment)
    ST(Geometry)
    ST(TessellationControl)
    ST(TessellationEvaluation)
    ST(Compute)
#undef ST

    if (types.isEmpty())
        return QStringLiteral("<none>");
    return types.join(QStringLiteral(" | "));
}
#endif // QT_NO_OPENGL

void OpenGLSupport::registerVariantHandler()
{
#ifndef QT_NO_OPENGL
    VariantHandler::registerStringConverter<QOpenGLShader::ShaderType>(shaderTypeToString);
#endif
}

OpenGLSupportFactory::OpenGLSupportFactory(QObject *parent)
    : QObject(parent)
{
}
