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
    MO_ADD_PROPERTY_ST(QGuiApplication, Qt::ApplicationState, applicationState);
    MO_ADD_PROPERTY_ST(QGuiApplication, bool, desktopSettingsAware);
    MO_ADD_PROPERTY_RO(QGuiApplication, qreal, devicePixelRatio);
    MO_ADD_PROPERTY_ST(QGuiApplication, QObject *, focusObject);
    MO_ADD_PROPERTY_ST(QGuiApplication, QWindow *, focusWindow);
    MO_ADD_PROPERTY_ST(QGuiApplication, QFont, font);
    MO_ADD_PROPERTY_ST(QGuiApplication, bool, isLeftToRight);
    MO_ADD_PROPERTY_ST(QGuiApplication, bool, isRightToLeft);
    MO_ADD_PROPERTY_ST(QGuiApplication, QPalette, palette);
    MO_ADD_PROPERTY_ST(QGuiApplication, QScreen *, primaryScreen);
#ifndef QT_NO_SESSIONMANAGER
    MO_ADD_PROPERTY_RO(QGuiApplication, bool, isSavingSession);
    MO_ADD_PROPERTY_RO(QGuiApplication, bool, isSessionRestored);
    MO_ADD_PROPERTY_RO(QGuiApplication, QString, sessionId);
    MO_ADD_PROPERTY_RO(QGuiApplication, QString, sessionKey);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 4, 0)
    MO_ADD_PROPERTY_ST(QGuiApplication, QWindowList, allWindows);
    MO_ADD_PROPERTY_ST(QGuiApplication, QWindowList, topLevelWindows);
#endif
#endif

    MO_ADD_METAOBJECT0(QPaintDevice);
    MO_ADD_PROPERTY_RO(QPaintDevice, int, colorCount);
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    MO_ADD_PROPERTY_RO(QPaintDevice, int, devicePixelRatio);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    MO_ADD_PROPERTY_RO(QPaintDevice, qreal, devicePixelRatioF);
#endif
    MO_ADD_PROPERTY_RO(QPaintDevice, int, heightMM);
    MO_ADD_PROPERTY_RO(QPaintDevice, int, logicalDpiX);
    MO_ADD_PROPERTY_RO(QPaintDevice, int, logicalDpiY);
    MO_ADD_PROPERTY_RO(QPaintDevice, bool, paintingActive);
    MO_ADD_PROPERTY_RO(QPaintDevice, int, physicalDpiX);
    MO_ADD_PROPERTY_RO(QPaintDevice, int, physicalDpiY);
    MO_ADD_PROPERTY_RO(QPaintDevice, int, widthMM);

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    qRegisterMetaType<QScreen *>();

    MO_ADD_METAOBJECT0(QSurface);
    MO_ADD_PROPERTY_RO(QSurface, QSurfaceFormat, format);
    MO_ADD_PROPERTY_RO(QSurface, QSize, size);
    MO_ADD_PROPERTY_RO(QSurface, QSurface::SurfaceClass, surfaceClass);
    MO_ADD_PROPERTY_RO(QSurface, QSurface::SurfaceType, surfaceType);

    MO_ADD_METAOBJECT2(QWindow, QObject, QSurface);
    MO_ADD_PROPERTY_CR(QWindow, QSize, baseSize, setBaseSize);
#ifndef QT_NO_CURSOR
    MO_ADD_PROPERTY_CR(QWindow, QCursor, cursor, setCursor);
#endif
    MO_ADD_PROPERTY_RO(QWindow, qreal, devicePixelRatio);
    MO_ADD_PROPERTY_CR(QWindow, QString, filePath, setFilePath);
    MO_ADD_PROPERTY_RO(QWindow, QObject *, focusObject);
    MO_ADD_PROPERTY_RO(QWindow, QRect, frameGeometry);
    MO_ADD_PROPERTY_RO(QWindow, QMargins, frameMargins);
    MO_ADD_PROPERTY_CR(QWindow, QPoint, framePosition, setFramePosition);
    MO_ADD_PROPERTY_CR(QWindow, QRect, geometry, setGeometry);
    MO_ADD_PROPERTY_CR(QWindow, QIcon, icon, setIcon);
    MO_ADD_PROPERTY_RO(QWindow, bool, isExposed);
    MO_ADD_PROPERTY_RO(QWindow, bool, isTopLevel);
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
    MO_ADD_PROPERTY_CR(QWindow, QRegion, mask, setMask);
#endif
    MO_ADD_PROPERTY_CR(QWindow, QPoint, position, setPosition);
    MO_ADD_PROPERTY_RO(QWindow, QSurfaceFormat, requestedFormat);
    MO_ADD_PROPERTY_RO(QWindow, QScreen *, screen);
    MO_ADD_PROPERTY_CR(QWindow, QSize, sizeIncrement, setSizeIncrement);
    MO_ADD_PROPERTY(QWindow, Qt::WindowState, windowState, setWindowState);
    MO_ADD_PROPERTY_RO(QWindow, QWindow *, transientParent);
    MO_ADD_PROPERTY_RO(QWindow, Qt::WindowType, type);

#ifndef QT_NO_OPENGL
    MO_ADD_METAOBJECT1(QOpenGLShader, QObject);
    MO_ADD_PROPERTY_RO(QOpenGLShader, bool, isCompiled);
    MO_ADD_PROPERTY_RO(QOpenGLShader, QString, log);
    MO_ADD_PROPERTY_RO(QOpenGLShader, uint, shaderId);
    MO_ADD_PROPERTY_RO(QOpenGLShader, QOpenGLShader::ShaderType, shaderType);
    MO_ADD_PROPERTY_RO(QOpenGLShader, QByteArray, sourceCode);

    MO_ADD_METAOBJECT1(QOpenGLShaderProgram, QObject);
    MO_ADD_PROPERTY_RO(QOpenGLShaderProgram, bool, isLinked);
    MO_ADD_PROPERTY_RO(QOpenGLShaderProgram, QString, log);
#if QT_VERSION >= QT_VERSION_CHECK(5, 1, 0)
// FIXME calling this asserts in debug builds of some newer Qt versions
// MO_ADD_PROPERTY_RO(QOpenGLShaderProgram, int, maxGeometryOutputVertices);
    MO_ADD_PROPERTY(QOpenGLShaderProgram, int, patchVertexCount, setPatchVertexCount);
#endif
    MO_ADD_PROPERTY_RO(QOpenGLShaderProgram, uint, programId);

    MO_ADD_METAOBJECT1(QOpenGLContext, QObject);
    MO_ADD_PROPERTY_RO(QOpenGLContext, uint, defaultFramebufferObject);
    // crashes if context isn't current
// MO_ADD_PROPERTY_RO(QOpenGLContext, QSet<QByteArray>, extensions);
    MO_ADD_PROPERTY_RO(QOpenGLContext, QSurfaceFormat, format);
    MO_ADD_PROPERTY_RO(QOpenGLContext, bool, isValid);
    MO_ADD_PROPERTY_RO(QOpenGLContext, QScreen *, screen);
    MO_ADD_PROPERTY_RO(QOpenGLContext, QOpenGLContext *, shareContext);
    MO_ADD_PROPERTY_RO(QOpenGLContext, QOpenGLContextGroup *, shareGroup);
// MO_ADD_PROPERTY_RO(QOpenGLContext, QSurface*, surface);
#endif // QT_NO_OPENGL
#endif // QT_VERSION >= 5.0.0

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    MO_ADD_METAOBJECT0(QBrush);
    MO_ADD_PROPERTY(QBrush, const QColor &, color, setColor);
    MO_ADD_PROPERTY_RO(QBrush, bool, isOpaque);
    MO_ADD_PROPERTY(QBrush, const QMatrix &, matrix, setMatrix);
    MO_ADD_PROPERTY(QBrush, Qt::BrushStyle, style, setStyle);
    MO_ADD_PROPERTY_CR(QBrush, QPixmap, texture, setTexture);
    MO_ADD_PROPERTY_CR(QBrush, QTransform, transform, setTransform);

    MO_ADD_METAOBJECT0(QPen);
    MO_ADD_PROPERTY_CR(QPen, QBrush, brush, setBrush);
    MO_ADD_PROPERTY(QPen, Qt::PenCapStyle, capStyle, setCapStyle);
    MO_ADD_PROPERTY_CR(QPen, QColor, color, setColor);
    MO_ADD_PROPERTY(QPen, qreal, dashOffset, setDashOffset);
    MO_ADD_PROPERTY_CR(QPen, QVector<qreal>, dashPattern, setDashPattern);
    MO_ADD_PROPERTY(QPen, bool, isCosmetic, setCosmetic);
    MO_ADD_PROPERTY_RO(QPen, bool, isSolid);
    MO_ADD_PROPERTY(QPen, Qt::PenJoinStyle, joinStyle, setJoinStyle);
    MO_ADD_PROPERTY(QPen, qreal, miterLimit, setMiterLimit);
    MO_ADD_PROPERTY(QPen, Qt::PenStyle, style, setStyle);
    MO_ADD_PROPERTY(QPen, int, width, setWidth);
    MO_ADD_PROPERTY(QPen, qreal, widthF, setWidthF);
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

    s += " Depth: " + QString::number(format.depthBufferSize());
    s += " Stencil: " + QString::number(format.stencilBufferSize());

    s += QStringLiteral(" Buffer: ");
    switch (format.swapBehavior()) {
    case QSurfaceFormat::DefaultSwapBehavior:
        s += QStringLiteral("default");
        break;
    case QSurfaceFormat::SingleBuffer:
        s += QStringLiteral("single");
        break;
    case QSurfaceFormat::DoubleBuffer:
        s += QStringLiteral("double");
        break;
    case QSurfaceFormat::TripleBuffer:
        s += QStringLiteral("triple");
        break;
    default:
        s += QStringLiteral("unknown");
    }

    return s;
}

static QString surfaceClassToString(QSurface::SurfaceClass sc)
{
    switch (sc) {
    case QSurface::Window:
        return QStringLiteral("Window");
#if QT_VERSION > QT_VERSION_CHECK(5, 1, 0)
    case QSurface::Offscreen:
        return QStringLiteral("Offscreen");
#endif
    }
    return QStringLiteral("Unknown Surface Class");
}

static QString surfaceTypeToString(QSurface::SurfaceType type)
{
    switch (type) {
    case QSurface::RasterSurface:
        return QStringLiteral("Raster");
    case QSurface::OpenGLSurface:
        return QStringLiteral("OpenGL");
#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)
    case QSurface::RasterGLSurface:
        return QStringLiteral("RasterGLSurface");
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    case QSurface::OpenVGSurface:
        return QStringLiteral("OpenVG");
#endif
    }
    return QStringLiteral("Unknown Surface Type");
}

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

void GuiSupport::registerVariantHandler()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    VariantHandler::registerStringConverter<QSurfaceFormat>(surfaceFormatToString);
    VariantHandler::registerStringConverter<QSurface::SurfaceClass>(surfaceClassToString);
    VariantHandler::registerStringConverter<QSurface::SurfaceType>(surfaceTypeToString);
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
