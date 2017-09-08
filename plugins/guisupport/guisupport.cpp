/*
  guisupport.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "guisupport.h"

#include <core/metaenum.h>
#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/varianthandler.h>

#include <common/metatypedeclarations.h>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QGuiApplication>
#include <QOpenGLContext>
#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QScreen>
#include <QWindow>
#endif

#include <QFont>
#include <QPaintDevice>
#include <QPainterPath>
#include <QPalette>
#include <QPen>
#include <QTextFormat>

using namespace GammaRay;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
Q_DECLARE_METATYPE(QSurface::SurfaceClass)
Q_DECLARE_METATYPE(QSurface::SurfaceType)
Q_DECLARE_METATYPE(QSurfaceFormat::FormatOptions)
#endif

GuiSupport::GuiSupport(GammaRay::ProbeInterface *probe, QObject *parent)
    : QObject(parent)
    , m_probe(probe)
{
    registerMetaTypes();
    registerVariantHandler();

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    connect(m_probe->probe(), SIGNAL(objectCreated(QObject*)), SLOT(objectCreated(QObject*)));
#endif
}

void GuiSupport::registerMetaTypes()
{
    MetaObject *mo;

 #if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    MO_ADD_METAOBJECT1(QGuiApplication, QCoreApplication);
    MO_ADD_PROPERTY_ST(QGuiApplication, applicationState);
    MO_ADD_PROPERTY_ST(QGuiApplication, desktopSettingsAware);
    MO_ADD_PROPERTY_RO(QGuiApplication, devicePixelRatio);
    MO_ADD_PROPERTY_ST(QGuiApplication, focusObject);
    MO_ADD_PROPERTY_ST(QGuiApplication, focusWindow);
    MO_ADD_PROPERTY_ST(QGuiApplication, font);
    MO_ADD_PROPERTY_ST(QGuiApplication, isLeftToRight);
    MO_ADD_PROPERTY_ST(QGuiApplication, isRightToLeft);
    MO_ADD_PROPERTY_ST(QGuiApplication, palette);
    MO_ADD_PROPERTY_ST(QGuiApplication, primaryScreen);
#ifndef QT_NO_SESSIONMANAGER
    MO_ADD_PROPERTY_RO(QGuiApplication, isSavingSession);
    MO_ADD_PROPERTY_RO(QGuiApplication, isSessionRestored);
    MO_ADD_PROPERTY_RO(QGuiApplication, sessionId);
    MO_ADD_PROPERTY_RO(QGuiApplication, sessionKey);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    MO_ADD_PROPERTY_ST(QGuiApplication, allWindows);
    MO_ADD_PROPERTY_ST(QGuiApplication, topLevelWindows);
#endif
#endif

    MO_ADD_METAOBJECT0(QPaintDevice);
    MO_ADD_PROPERTY_RO(QPaintDevice, colorCount);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    MO_ADD_PROPERTY_RO(QPaintDevice, devicePixelRatio);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    MO_ADD_PROPERTY_RO(QPaintDevice, devicePixelRatioF);
#endif
    MO_ADD_PROPERTY_RO(QPaintDevice, heightMM);
    MO_ADD_PROPERTY_RO(QPaintDevice, logicalDpiX);
    MO_ADD_PROPERTY_RO(QPaintDevice, logicalDpiY);
    MO_ADD_PROPERTY_RO(QPaintDevice, paintingActive);
    MO_ADD_PROPERTY_RO(QPaintDevice, physicalDpiX);
    MO_ADD_PROPERTY_RO(QPaintDevice, physicalDpiY);
    MO_ADD_PROPERTY_RO(QPaintDevice, widthMM);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    qRegisterMetaType<QScreen *>();

    MO_ADD_METAOBJECT0(QSurface);
    MO_ADD_PROPERTY_RO(QSurface, format);
    MO_ADD_PROPERTY_RO(QSurface, size);
    MO_ADD_PROPERTY_RO(QSurface, supportsOpenGL);
    MO_ADD_PROPERTY_RO(QSurface, surfaceClass);
    MO_ADD_PROPERTY_RO(QSurface, surfaceType);

#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0) // not a gadget previously
    MO_ADD_METAOBJECT0(QSurfaceFormat);
    MO_ADD_PROPERTY(QSurfaceFormat, alphaBufferSize, setAlphaBufferSize);
    MO_ADD_PROPERTY(QSurfaceFormat, blueBufferSize, setBlueBufferSize);
    MO_ADD_PROPERTY_RO(QSurfaceFormat, defaultFormat);
    MO_ADD_PROPERTY(QSurfaceFormat, depthBufferSize, setDepthBufferSize);
    MO_ADD_PROPERTY(QSurfaceFormat, greenBufferSize, setGreenBufferSize);
    MO_ADD_PROPERTY_RO(QSurfaceFormat, hasAlpha);
    MO_ADD_PROPERTY(QSurfaceFormat, majorVersion, setMajorVersion);
    MO_ADD_PROPERTY(QSurfaceFormat, minorVersion, setMinorVersion);
    MO_ADD_PROPERTY(QSurfaceFormat, options, setOptions);
    MO_ADD_PROPERTY(QSurfaceFormat, profile, setProfile);
    MO_ADD_PROPERTY(QSurfaceFormat, redBufferSize, setRedBufferSize);
    MO_ADD_PROPERTY(QSurfaceFormat, renderableType, setRenderableType);
    MO_ADD_PROPERTY(QSurfaceFormat, samples, setSamples);
    MO_ADD_PROPERTY(QSurfaceFormat, stencilBufferSize, setStencilBufferSize);
    MO_ADD_PROPERTY(QSurfaceFormat, stereo, setStereo);
    MO_ADD_PROPERTY(QSurfaceFormat, swapBehavior, setSwapBehavior);
    MO_ADD_PROPERTY(QSurfaceFormat, swapInterval, setSwapInterval);
#endif

    MO_ADD_METAOBJECT2(QWindow, QObject, QSurface);
    MO_ADD_PROPERTY(QWindow, baseSize, setBaseSize);
#ifndef QT_NO_CURSOR
    MO_ADD_PROPERTY(QWindow, cursor, setCursor);
#endif
    MO_ADD_PROPERTY_RO(QWindow, devicePixelRatio);
    MO_ADD_PROPERTY(QWindow, filePath, setFilePath);
    MO_ADD_PROPERTY_RO(QWindow,  focusObject);
    MO_ADD_PROPERTY_RO(QWindow, frameGeometry);
    MO_ADD_PROPERTY_RO(QWindow, frameMargins);
    MO_ADD_PROPERTY(QWindow, framePosition, setFramePosition);
    MO_ADD_PROPERTY(QWindow, geometry, setGeometry);
    MO_ADD_PROPERTY(QWindow, icon, setIcon);
    MO_ADD_PROPERTY_RO(QWindow, isExposed);
    MO_ADD_PROPERTY_RO(QWindow, isTopLevel);
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    MO_ADD_PROPERTY(QWindow, mask, setMask);
#endif
    MO_ADD_PROPERTY(QWindow, position, setPosition);
    MO_ADD_PROPERTY_RO(QWindow, requestedFormat);
    MO_ADD_PROPERTY_RO(QWindow, screen);
    MO_ADD_PROPERTY(QWindow, sizeIncrement, setSizeIncrement);
    MO_ADD_PROPERTY(QWindow, windowState, setWindowState);
    MO_ADD_PROPERTY_RO(QWindow, transientParent);
    MO_ADD_PROPERTY_RO(QWindow, type);

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
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
// FIXME calling this asserts in debug builds of some newer Qt versions
// MO_ADD_PROPERTY_RO(QOpenGLShaderProgram, maxGeometryOutputVertices);
    MO_ADD_PROPERTY(QOpenGLShaderProgram, patchVertexCount, setPatchVertexCount);
#endif
    MO_ADD_PROPERTY_RO(QOpenGLShaderProgram, programId);

    MO_ADD_METAOBJECT1(QOpenGLContext, QObject);
    MO_ADD_PROPERTY_RO(QOpenGLContext, defaultFramebufferObject);
    // crashes if context isn't current
// MO_ADD_PROPERTY_RO(QOpenGLContext, extensions);
    MO_ADD_PROPERTY_RO(QOpenGLContext, format);
    MO_ADD_PROPERTY_RO(QOpenGLContext, isValid);
    MO_ADD_PROPERTY_RO(QOpenGLContext, screen);
    MO_ADD_PROPERTY_RO(QOpenGLContext, shareContext);
    MO_ADD_PROPERTY_RO(QOpenGLContext, shareGroup);
// MO_ADD_PROPERTY_RO(QOpenGLContext, surface);
#endif // QT_NO_OPENGL
#endif // QT_VERSION >= 5.0.0

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    MO_ADD_METAOBJECT0(QBrush);
    MO_ADD_PROPERTY_O2(QBrush, color, setColor);
    MO_ADD_PROPERTY_RO(QBrush, isOpaque);
    MO_ADD_PROPERTY(QBrush, matrix, setMatrix);
    MO_ADD_PROPERTY(QBrush, style, setStyle);
    MO_ADD_PROPERTY(QBrush, texture, setTexture);
    MO_ADD_PROPERTY(QBrush, transform, setTransform);

    MO_ADD_METAOBJECT0(QIcon);
    MO_ADD_PROPERTY_RO(QIcon, cacheKey);
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    MO_ADD_PROPERTY(QIcon, isMask, setIsMask);
#endif
    MO_ADD_PROPERTY_RO(QIcon, isNull);
    MO_ADD_PROPERTY_RO(QIcon, name);
    MO_ADD_PROPERTY_RO(QIcon, themeName);
    MO_ADD_PROPERTY_RO(QIcon, themeSearchPaths);

    MO_ADD_METAOBJECT0(QPen);
    MO_ADD_PROPERTY(QPen, brush, setBrush);
    MO_ADD_PROPERTY(QPen, capStyle, setCapStyle);
    MO_ADD_PROPERTY(QPen, color, setColor);
    MO_ADD_PROPERTY(QPen, dashOffset, setDashOffset);
    MO_ADD_PROPERTY(QPen, dashPattern, setDashPattern);
    MO_ADD_PROPERTY(QPen, isCosmetic, setCosmetic);
    MO_ADD_PROPERTY_RO(QPen, isSolid);
    MO_ADD_PROPERTY(QPen, joinStyle, setJoinStyle);
    MO_ADD_PROPERTY(QPen, miterLimit, setMiterLimit);
    MO_ADD_PROPERTY(QPen, style, setStyle);
    MO_ADD_PROPERTY(QPen, width, setWidth);
    MO_ADD_PROPERTY(QPen, widthF, setWidthF);
#endif
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
static QString surfaceFormatToString(const QSurfaceFormat &format)
{
    QString s;
    switch (format.renderableType()) {
    case QSurfaceFormat::DefaultRenderableType:
        s += QStringLiteral("Default");
        break;
    case QSurfaceFormat::OpenGL:
        s += QStringLiteral("OpenGL");
        break;
    case QSurfaceFormat::OpenGLES:
        s += QStringLiteral("OpenGL ES");
        break;
    case QSurfaceFormat::OpenVG:
        s += QStringLiteral("OpenVG");
        break;
    }

    s += " (" + QString::number(format.majorVersion())
         +'.' + QString::number(format.minorVersion());
    switch (format.profile()) {
    case QSurfaceFormat::CoreProfile:
        s += QStringLiteral(" core");
        break;
    case QSurfaceFormat::CompatibilityProfile:
        s += QStringLiteral(" compat");
        break;
    case QSurfaceFormat::NoProfile:
        break;
    }
    s += ')';

    s += " RGBA: " + QString::number(format.redBufferSize())
         +'/' + QString::number(format.greenBufferSize())
         +'/' + QString::number(format.blueBufferSize())
         +'/' + QString::number(format.alphaBufferSize());

    return s;
}

#define E(x) { QSurface:: x, #x }
static const MetaEnum::Value<QSurface::SurfaceClass> surface_class_table[] = {
    E(Window),
    E(Offscreen)
};

static const MetaEnum::Value<QSurface::SurfaceType> surface_type_table[] = {
    E(RasterSurface),
    E(OpenGLSurface),
    E(RasterGLSurface),
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    E(OpenVGSurface),
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    E(VulkanSurface)
#endif
};
#undef E

#ifndef QT_NO_OPENGL
static QString shaderTypeToString(const QOpenGLShader::ShaderType type)
{
    QStringList types;
#define ST(t) if (type & QOpenGLShader::t) types.push_back(QStringLiteral(#t));
    ST(Vertex)
    ST(Fragment)
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    ST(Geometry)
    ST(TessellationControl)
    ST(TessellationEvaluation)
    ST(Compute)
#endif
#undef ST

    if (types.isEmpty())
        return QStringLiteral("<none>");
    return types.join(QStringLiteral(" | "));
}
#endif // QT_NO_OPENGL
#endif

static QString textLengthToString(const QTextLength &l)
{
    QString typeStr;
    switch (l.type()) {
    case QTextLength::VariableLength:
        typeStr = GuiSupport::tr("variable");
        break;
    case QTextLength::FixedLength:
        typeStr = GuiSupport::tr("fixed");
        break;
    case QTextLength::PercentageLength:
        typeStr = GuiSupport::tr("percentage");
        break;
    }
    return QStringLiteral("%1 (%2)").arg(l.rawValue()).arg(typeStr);
}

static QString painterPathToString(const QPainterPath &path)
{
    if (path.isEmpty())
        return GuiSupport::tr("<empty>");
    return GuiSupport::tr("<%1 elements>").arg(path.elementCount());
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#define E(x) { QSurfaceFormat:: x, #x }
static const MetaEnum::Value<QSurfaceFormat::FormatOption> surface_format_option_table[] = {
    E(StereoBuffers),
    E(DebugContext),
    E(DeprecatedFunctions),
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    E(ResetNotification)
#endif
};
#undef E
#endif

void GuiSupport::registerVariantHandler()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    VariantHandler::registerStringConverter<QSurfaceFormat>(surfaceFormatToString);
    VariantHandler::registerStringConverter<QSurface::SurfaceClass>(MetaEnum::enumToString_fn(surface_class_table));
    VariantHandler::registerStringConverter<QSurface::SurfaceType>(MetaEnum::enumToString_fn(surface_type_table));
    VariantHandler::registerStringConverter<QSurfaceFormat::FormatOptions>(MetaEnum::flagsToString_fn(surface_format_option_table));
#ifndef QT_NO_OPENGL
    VariantHandler::registerStringConverter<QOpenGLShader::ShaderType>(shaderTypeToString);
#endif
#endif

    VariantHandler::registerStringConverter<QPainterPath>(painterPathToString);
    VariantHandler::registerStringConverter<QTextLength>(textLengthToString);
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
void GuiSupport::discoverObjects()
{
    foreach (QWindow *window, qApp->topLevelWindows())
        m_probe->discoverObject(window);
}

void GuiSupport::objectCreated(QObject *object)
{
    if (qobject_cast<QGuiApplication *>(object))
        discoverObjects();
}
#endif

GuiSupportFactory::GuiSupportFactory(QObject *parent)
    : QObject(parent)
{
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN(GuiSupportFactory)
#endif
