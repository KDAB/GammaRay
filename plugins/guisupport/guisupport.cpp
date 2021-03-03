/*
  guisupport.cpp

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

#include "guisupport.h"

#include <core/enumrepositoryserver.h>
#include <core/enumutil.h>
#include <core/metaenum.h>
#include <core/metaobject.h>
#include <core/metaobjectrepository.h>
#include <core/util.h>
#include <core/varianthandler.h>

#include <common/metatypedeclarations.h>

#include <QGuiApplication>
#include <QOpenGLContext>
#include <QScreen>
#include <QWindow>

#include <qpa/qplatformpixmap.h>

#include <QClipboard>
#include <QIcon>
#include <QInputEvent>
#include <QFont>
#include <QMimeData>
#include <QMouseEvent>
#include <QPaintDevice>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QPen>
#include <QTextFormat>
#include <QTimer>

using namespace GammaRay;

Q_DECLARE_METATYPE(QFont::Capitalization)
Q_DECLARE_METATYPE(QFont::HintingPreference)
Q_DECLARE_METATYPE(QFont::SpacingType)
Q_DECLARE_METATYPE(QFont::Style)
Q_DECLARE_METATYPE(QFont::StyleHint)
Q_DECLARE_METATYPE(QImage*)
Q_DECLARE_METATYPE(QPlatformPixmap*)
Q_DECLARE_METATYPE(QPlatformPixmap::ClassId)
Q_DECLARE_METATYPE(QSurface::SurfaceClass)
Q_DECLARE_METATYPE(QSurface::SurfaceType)
Q_DECLARE_METATYPE(QSurfaceFormat::FormatOptions)
Q_DECLARE_METATYPE(const QMimeData*)
Q_DECLARE_METATYPE(QImage::Format)
Q_DECLARE_METATYPE(const QGradient*)
Q_DECLARE_METATYPE(QPixelFormat)
Q_DECLARE_METATYPE(QPixelFormat::AlphaUsage)
Q_DECLARE_METATYPE(QPixelFormat::AlphaPosition)
Q_DECLARE_METATYPE(QPixelFormat::AlphaPremultiplied)
Q_DECLARE_METATYPE(QPixelFormat::ByteOrder)
Q_DECLARE_METATYPE(QPixelFormat::ColorModel)
Q_DECLARE_METATYPE(QPixelFormat::TypeInterpretation)
Q_DECLARE_METATYPE(QPixelFormat::YUVLayout)
Q_DECLARE_METATYPE(Qt::MouseButton)
Q_DECLARE_METATYPE(QFlags<Qt::MouseEventFlag>)
Q_DECLARE_METATYPE(QTouchEvent::TouchPoint)
Q_DECLARE_METATYPE(QList<QTouchEvent::TouchPoint>)
Q_DECLARE_METATYPE(Qt::TouchPointState)
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
Q_DECLARE_METATYPE(QFlags<QTouchEvent::TouchPoint::InfoFlag>)
Q_DECLARE_METATYPE(QFlags<QTouchDevice::CapabilityFlag>)
Q_DECLARE_METATYPE(QTouchDevice*)
Q_DECLARE_METATYPE(const QTouchDevice*)
#endif
Q_DECLARE_METATYPE(QScrollEvent::ScrollState)
Q_DECLARE_METATYPE(QList<QInputMethodEvent::Attribute>)
Q_DECLARE_METATYPE(QContextMenuEvent::Reason)


// QGradient is pseudo-polymorphic, make it introspectable nevertheless
#define MAKE_GRADIENT_CAST(Type) \
    template<> Q ## Type *DynamicCast<Q ## Type *>(QGradient *g) { \
        if (g->type() == QGradient:: Type) return static_cast<Q ## Type *>(g); \
        return nullptr; \
    }

namespace GammaRay
{
MAKE_GRADIENT_CAST(ConicalGradient)
MAKE_GRADIENT_CAST(LinearGradient)
MAKE_GRADIENT_CAST(RadialGradient)

template <>
bool IsPolymorphic<QGradient>() { return true; }
}
#undef MAKE_GRADIENT_CAST

static bool isAcceptableWindow(QWindow *w)
{
    return w
            && w->isTopLevel()
            && w->surfaceClass() != QSurface::Offscreen
            // Offscreen windows can have a surface different than Offscreen,
            // but they contains a window title 'Offscreen'
            && w->title() != QStringLiteral("Offscreen")
    ;
}

GuiSupport::GuiSupport(Probe *probe, QObject *parent)
    : QObject(parent)
    , m_probe(probe)
{
    registerMetaTypes();
    registerVariantHandler();

    m_iconAndTitleOverrider.titleSuffix = tr(" (Injected by GammaRay)");
    connect(m_probe, &Probe::objectCreated, this, &GuiSupport::objectCreated);

    if (auto guiApp = qobject_cast<QGuiApplication*>(QCoreApplication::instance())) {
        QTimer::singleShot(0, this, [this]() {
            updateWindowIcon();
        });

        m_probe->installGlobalEventFilter(this);
        foreach (auto w, guiApp->topLevelWindows()) {
            if (isAcceptableWindow(w))
                updateWindowTitle(w);
        }
        connect(m_probe, &Probe::aboutToDetach, this, &GuiSupport::restoreIconAndTitle, Qt::DirectConnection);
    }
}

void GuiSupport::registerMetaTypes()
{
    MetaObject *mo;

    MO_ADD_METAOBJECT1(QMimeData, QObject);
    MO_ADD_PROPERTY   (QMimeData, colorData, setColorData);
    MO_ADD_PROPERTY_RO(QMimeData, formats);
    MO_ADD_PROPERTY_RO(QMimeData, hasColor);
    MO_ADD_PROPERTY_RO(QMimeData, hasHtml);
    MO_ADD_PROPERTY_RO(QMimeData, hasText);
    MO_ADD_PROPERTY_RO(QMimeData, hasUrls);
    MO_ADD_PROPERTY   (QMimeData, html, setHtml);
    MO_ADD_PROPERTY   (QMimeData, imageData, setImageData);
    MO_ADD_PROPERTY   (QMimeData, text, setText);
    MO_ADD_PROPERTY   (QMimeData, urls, setUrls);

#ifndef QT_NO_CLIPBOARD
    MO_ADD_METAOBJECT1(QClipboard, QObject);
    MO_ADD_PROPERTY_LD(QClipboard, clipboardMimeData, [](QClipboard *cb) { return cb->mimeData(QClipboard::Clipboard); });
    MO_ADD_PROPERTY_LD(QClipboard, findBufferMimeData, [](QClipboard *cb) { return cb->mimeData(QClipboard::FindBuffer); });
    MO_ADD_PROPERTY_RO(QClipboard, ownsClipboard);
    MO_ADD_PROPERTY_RO(QClipboard, ownsFindBuffer);
    MO_ADD_PROPERTY_RO(QClipboard, ownsSelection);
    MO_ADD_PROPERTY_LD(QClipboard, selectionMimeData, [](QClipboard *cb) { return cb->mimeData(QClipboard::Selection); });
    MO_ADD_PROPERTY_RO(QClipboard, supportsFindBuffer);
    MO_ADD_PROPERTY_RO(QClipboard, supportsSelection);
#endif

    MO_ADD_METAOBJECT1(QGuiApplication, QCoreApplication);
    MO_ADD_PROPERTY_ST(QGuiApplication, applicationState);
#ifndef QT_NO_CLIPBOARD
    MO_ADD_PROPERTY_ST(QGuiApplication, clipboard);
#endif
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
    MO_ADD_PROPERTY_ST(QGuiApplication, allWindows);
    MO_ADD_PROPERTY_ST(QGuiApplication, topLevelWindows);

    MO_ADD_METAOBJECT0(QPaintDevice);
    MO_ADD_PROPERTY_RO(QPaintDevice, colorCount);
    MO_ADD_PROPERTY_RO(QPaintDevice, depth);
    MO_ADD_PROPERTY_RO(QPaintDevice, devicePixelRatio);
#if QT_VERSION >= QT_VERSION_CHECK(5, 6, 0)
    MO_ADD_PROPERTY_RO(QPaintDevice, devicePixelRatioF);
#endif
    MO_ADD_PROPERTY_RO(QPaintDevice, height);
    MO_ADD_PROPERTY_RO(QPaintDevice, heightMM);
    MO_ADD_PROPERTY_RO(QPaintDevice, logicalDpiX);
    MO_ADD_PROPERTY_RO(QPaintDevice, logicalDpiY);
    MO_ADD_PROPERTY_RO(QPaintDevice, paintingActive);
    MO_ADD_PROPERTY_RO(QPaintDevice, physicalDpiX);
    MO_ADD_PROPERTY_RO(QPaintDevice, physicalDpiY);
    MO_ADD_PROPERTY_RO(QPaintDevice, width);
    MO_ADD_PROPERTY_RO(QPaintDevice, widthMM);

    MO_ADD_METAOBJECT1(QImage, QPaintDevice);
    MO_ADD_PROPERTY_RO(QImage, allGray);
    MO_ADD_PROPERTY_RO(QImage, bitPlaneCount);
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    MO_ADD_PROPERTY_RO(QImage, sizeInBytes)
#else
    MO_ADD_PROPERTY_RO(QImage, byteCount);
#endif
    MO_ADD_PROPERTY_RO(QImage, bytesPerLine);
    MO_ADD_PROPERTY_RO(QImage, cacheKey);
    MO_ADD_PROPERTY   (QImage, dotsPerMeterX, setDotsPerMeterX);
    MO_ADD_PROPERTY   (QImage, dotsPerMeterY, setDotsPerMeterY);
    MO_ADD_PROPERTY_RO(QImage, format);
    MO_ADD_PROPERTY_RO(QImage, hasAlphaChannel);
    MO_ADD_PROPERTY_RO(QImage, isGrayscale);
    MO_ADD_PROPERTY_RO(QImage, isNull);
    MO_ADD_PROPERTY   (QImage, offset, setOffset);
    MO_ADD_PROPERTY_RO(QImage, pixelFormat);
    MO_ADD_PROPERTY_RO(QImage, rect);
    MO_ADD_PROPERTY_RO(QImage, size);
    MO_ADD_PROPERTY_RO(QImage, textKeys);

    MO_ADD_METAOBJECT1(QPixmap, QPaintDevice);
    MO_ADD_PROPERTY_RO(QPixmap, cacheKey);
    MO_ADD_PROPERTY_RO(QPixmap, hasAlpha);
    MO_ADD_PROPERTY_RO(QPixmap, hasAlphaChannel);
    MO_ADD_PROPERTY_RO(QPixmap, isNull);
    MO_ADD_PROPERTY_RO(QPixmap, isQBitmap);
    MO_ADD_PROPERTY_RO(QPixmap, rect);
    MO_ADD_PROPERTY_RO(QPixmap, size);
    MO_ADD_PROPERTY_RO(QPixmap, handle);
    MO_ADD_METAOBJECT0(QPlatformPixmap);
    MO_ADD_PROPERTY_NC(QPlatformPixmap, buffer);
    MO_ADD_PROPERTY_RO(QPlatformPixmap, classId);

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
    MO_ADD_PROPERTY(QWindow, mask, setMask);
    MO_ADD_PROPERTY(QWindow, position, setPosition);
    MO_ADD_PROPERTY_RO(QWindow, requestedFormat);
    MO_ADD_PROPERTY_RO(QWindow, screen);
    MO_ADD_PROPERTY(QWindow, sizeIncrement, setSizeIncrement);
    MO_ADD_PROPERTY(QWindow, windowState, setWindowState);
    MO_ADD_PROPERTY_RO(QWindow, transientParent);
    MO_ADD_PROPERTY_RO(QWindow, type);

#ifndef QT_NO_OPENGL
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

    MO_ADD_METAOBJECT0(QGradient);
    MO_ADD_PROPERTY   (QGradient, coordinateMode, setCoordinateMode);
    MO_ADD_PROPERTY   (QGradient, spread, setSpread);
    MO_ADD_PROPERTY   (QGradient, stops, setStops);
    MO_ADD_PROPERTY_RO(QGradient, type);

    MO_ADD_METAOBJECT1(QConicalGradient, QGradient);
    MO_ADD_PROPERTY   (QConicalGradient, angle, setAngle);
    MO_ADD_PROPERTY_O2(QConicalGradient, center, setCenter);

    MO_ADD_METAOBJECT1(QLinearGradient, QGradient);
    MO_ADD_PROPERTY_O2(QLinearGradient, finalStop, setFinalStop);
    MO_ADD_PROPERTY_O2(QLinearGradient, start, setStart);

    MO_ADD_METAOBJECT1(QRadialGradient, QGradient);
    MO_ADD_PROPERTY_O2(QRadialGradient, center, setCenter);
    MO_ADD_PROPERTY   (QRadialGradient, centerRadius, setCenterRadius);
    MO_ADD_PROPERTY_O2(QRadialGradient, focalPoint, setFocalPoint);
    MO_ADD_PROPERTY   (QRadialGradient, focalRadius, setFocalRadius);
    MO_ADD_PROPERTY   (QRadialGradient, radius, setRadius);

    MO_ADD_METAOBJECT0(QBrush);
    MO_ADD_PROPERTY_O2(QBrush, color, setColor);
    MO_ADD_PROPERTY_RO(QBrush, gradient);
    MO_ADD_PROPERTY_RO(QBrush, isOpaque);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    MO_ADD_PROPERTY(QBrush, matrix, setMatrix);
#endif
    MO_ADD_PROPERTY(QBrush, style, setStyle);
    MO_ADD_PROPERTY(QBrush, texture, setTexture);
    MO_ADD_PROPERTY(QBrush, transform, setTransform);

    MO_ADD_METAOBJECT0(QFont);
    MO_ADD_PROPERTY(QFont, bold, setBold);
    MO_ADD_PROPERTY(QFont, capitalization, setCapitalization);
    MO_ADD_PROPERTY_RO(QFont, defaultFamily);
    MO_ADD_PROPERTY_RO(QFont, exactMatch);
    MO_ADD_PROPERTY(QFont, family, setFamily);
    MO_ADD_PROPERTY(QFont, fixedPitch, setFixedPitch);
    MO_ADD_PROPERTY(QFont, hintingPreference, setHintingPreference);
    MO_ADD_PROPERTY(QFont, italic, setItalic);
    MO_ADD_PROPERTY(QFont, kerning, setKerning);
    MO_ADD_PROPERTY_RO(QFont, key);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    MO_ADD_PROPERTY_RO(QFont, lastResortFamily);
#endif
//     MO_ADD_PROPERTY_RO(QFont, lastResortFont); asserts at runtime!?
    MO_ADD_PROPERTY_RO(QFont, letterSpacing);
    MO_ADD_PROPERTY_RO(QFont, letterSpacingType);
    MO_ADD_PROPERTY(QFont, overline, setOverline);
    MO_ADD_PROPERTY(QFont, pixelSize, setPixelSize);
    MO_ADD_PROPERTY(QFont, pointSize, setPointSize);
    MO_ADD_PROPERTY(QFont, pointSizeF, setPointSizeF);
    MO_ADD_PROPERTY(QFont, stretch, setStretch);
    MO_ADD_PROPERTY(QFont, strikeOut, setStrikeOut);
    MO_ADD_PROPERTY(QFont, style, setStyle);
    MO_ADD_PROPERTY_RO(QFont, styleHint);
    MO_ADD_PROPERTY(QFont, styleName, setStyleName);
    MO_ADD_PROPERTY(QFont, styleStrategy, setStyleStrategy);
    MO_ADD_PROPERTY_RO(QFont, substitutions);
    MO_ADD_PROPERTY(QFont, underline, setUnderline);
    MO_ADD_PROPERTY(QFont, weight, setWeight);
    MO_ADD_PROPERTY(QFont, wordSpacing, setWordSpacing);

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

    MO_ADD_METAOBJECT0(QPixelFormat);
    MO_ADD_PROPERTY_RO(QPixelFormat, alphaPosition);
    MO_ADD_PROPERTY_RO(QPixelFormat, alphaSize);
    MO_ADD_PROPERTY_RO(QPixelFormat, alphaUsage);
    MO_ADD_PROPERTY_RO(QPixelFormat, bitsPerPixel);
    MO_ADD_PROPERTY_RO(QPixelFormat, blackSize);
    MO_ADD_PROPERTY_RO(QPixelFormat, blueSize);
    MO_ADD_PROPERTY_RO(QPixelFormat, brightnessSize);
    MO_ADD_PROPERTY_RO(QPixelFormat, byteOrder);
    MO_ADD_PROPERTY_RO(QPixelFormat, channelCount);
    MO_ADD_PROPERTY_RO(QPixelFormat, colorModel);
    MO_ADD_PROPERTY_RO(QPixelFormat, cyanSize);
    MO_ADD_PROPERTY_RO(QPixelFormat, greenSize);
    MO_ADD_PROPERTY_RO(QPixelFormat, hueSize);
    MO_ADD_PROPERTY_RO(QPixelFormat, lightnessSize);
    MO_ADD_PROPERTY_RO(QPixelFormat, magentaSize);
    MO_ADD_PROPERTY_RO(QPixelFormat, premultiplied);
    MO_ADD_PROPERTY_RO(QPixelFormat, redSize);
    MO_ADD_PROPERTY_RO(QPixelFormat, saturationSize);
    MO_ADD_PROPERTY_RO(QPixelFormat, typeInterpretation);
    MO_ADD_PROPERTY_RO(QPixelFormat, yellowSize);
    MO_ADD_PROPERTY_RO(QPixelFormat, yuvLayout);

    MO_ADD_METAOBJECT1(QDropEvent, QEvent);
    MO_ADD_PROPERTY_RO(QDropEvent, dropAction);
    MO_ADD_PROPERTY_RO(QDropEvent, keyboardModifiers);
    MO_ADD_PROPERTY_RO(QDropEvent, mimeData);
    MO_ADD_PROPERTY_RO(QDropEvent, mouseButtons);
    MO_ADD_PROPERTY_RO(QDropEvent, pos);
    MO_ADD_PROPERTY_RO(QDropEvent, posF);
    MO_ADD_PROPERTY_RO(QDropEvent, possibleActions);
    MO_ADD_PROPERTY_RO(QDropEvent, proposedAction);
    MO_ADD_PROPERTY_RO(QDropEvent, source);

    MO_ADD_METAOBJECT1(QDragMoveEvent, QDropEvent);
    MO_ADD_PROPERTY_RO(QDragMoveEvent, answerRect);

    MO_ADD_METAOBJECT1(QInputEvent, QEvent);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    MO_ADD_PROPERTY_RO(QInputEvent, device);
    MO_ADD_PROPERTY_RO(QInputEvent, deviceType);
#endif
    MO_ADD_PROPERTY_RO(QInputEvent, modifiers);
    MO_ADD_PROPERTY_RO(QInputEvent, timestamp);

    MO_ADD_METAOBJECT1(QContextMenuEvent, QInputEvent);
    MO_ADD_PROPERTY_RO(QContextMenuEvent, globalPos);
    MO_ADD_PROPERTY_RO(QContextMenuEvent, pos);
    MO_ADD_PROPERTY_RO(QContextMenuEvent, reason);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    MO_ADD_METAOBJECT1(QPointerEvent, QInputEvent);
    MO_ADD_PROPERTY_RO(QPointerEvent, allPointsAccepted);
    MO_ADD_PROPERTY_RO(QPointerEvent, allPointsGrabbed);
    MO_ADD_PROPERTY_RO(QPointerEvent, pointCount);
    MO_ADD_PROPERTY_RO(QPointerEvent, pointerType);
    MO_ADD_PROPERTY_RO(QPointerEvent, pointingDevice);

    MO_ADD_METAOBJECT1(QSinglePointEvent, QPointerEvent);
    MO_ADD_PROPERTY_RO(QSinglePointEvent, button);
    MO_ADD_PROPERTY_RO(QSinglePointEvent, buttons);
    MO_ADD_PROPERTY_RO(QSinglePointEvent, exclusivePointGrabber);
    MO_ADD_PROPERTY_RO(QSinglePointEvent, globalPosition)
    MO_ADD_PROPERTY_RO(QSinglePointEvent, isBeginEvent);
    MO_ADD_PROPERTY_RO(QSinglePointEvent, isEndEvent);
    MO_ADD_PROPERTY_RO(QSinglePointEvent, isUpdateEvent);
    MO_ADD_PROPERTY_RO(QSinglePointEvent, position);
    MO_ADD_PROPERTY_RO(QSinglePointEvent, scenePosition);
#endif

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    MO_ADD_METAOBJECT1(QMouseEvent, QInputEvent);
    MO_ADD_PROPERTY_RO(QMouseEvent, button);
    MO_ADD_PROPERTY_RO(QMouseEvent, buttons);
    MO_ADD_PROPERTY_RO(QMouseEvent, globalPos);
    MO_ADD_PROPERTY_RO(QMouseEvent, pos);
#else
    MO_ADD_METAOBJECT1(QMouseEvent, QSinglePointEvent);
#endif
    MO_ADD_PROPERTY_RO(QMouseEvent, flags);
    MO_ADD_PROPERTY_RO(QMouseEvent, localPos);
    MO_ADD_PROPERTY_RO(QMouseEvent, screenPos);
    MO_ADD_PROPERTY_RO(QMouseEvent, source);
    MO_ADD_PROPERTY_RO(QMouseEvent, windowPos);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    MO_ADD_METAOBJECT1(QHoverEvent, QInputEvent);
    MO_ADD_PROPERTY_RO(QHoverEvent, posF);
#else
    MO_ADD_METAOBJECT1(QHoverEvent, QSinglePointEvent);
#endif
    MO_ADD_PROPERTY_RO(QHoverEvent, oldPosF);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    MO_ADD_METAOBJECT1(QWheelEvent, QInputEvent);
    MO_ADD_PROPERTY_RO(QWheelEvent, buttons);
    MO_ADD_PROPERTY_RO(QWheelEvent, delta);
    MO_ADD_PROPERTY_RO(QWheelEvent, orientation);
    MO_ADD_PROPERTY_RO(QWheelEvent, posF);
    MO_ADD_PROPERTY_RO(QWheelEvent, globalPosF);
#else
    MO_ADD_METAOBJECT1(QWheelEvent, QSinglePointEvent);
#endif
    MO_ADD_PROPERTY_RO(QWheelEvent, pixelDelta);
    MO_ADD_PROPERTY_RO(QWheelEvent, angleDelta);
    MO_ADD_PROPERTY_RO(QWheelEvent, phase);
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    MO_ADD_PROPERTY_RO(QWheelEvent, inverted);
#endif
    MO_ADD_PROPERTY_RO(QWheelEvent, source);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    MO_ADD_METAOBJECT1(QTabletEvent, QInputEvent);
    MO_ADD_PROPERTY_RO(QTabletEvent, device);
    MO_ADD_PROPERTY_RO(QTabletEvent, pointerType);
    MO_ADD_PROPERTY_RO(QTabletEvent, posF);
    MO_ADD_PROPERTY_RO(QTabletEvent, globalPosF);
#else
    MO_ADD_METAOBJECT1(QTabletEvent, QSinglePointEvent);
#endif
    MO_ADD_PROPERTY_RO(QTabletEvent, uniqueId);
    MO_ADD_PROPERTY_RO(QTabletEvent, pressure);
    MO_ADD_PROPERTY_RO(QTabletEvent, z);
    MO_ADD_PROPERTY_RO(QTabletEvent, tangentialPressure);
    MO_ADD_PROPERTY_RO(QTabletEvent, rotation);
    MO_ADD_PROPERTY_RO(QTabletEvent, xTilt);
    MO_ADD_PROPERTY_RO(QTabletEvent, yTilt);
    MO_ADD_PROPERTY_RO(QTabletEvent, button);
    MO_ADD_PROPERTY_RO(QTabletEvent, buttons);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    MO_ADD_METAOBJECT1(QNativeGestureEvent, QInputEvent);
#else
    MO_ADD_METAOBJECT1(QNativeGestureEvent, QSinglePointEvent);
#endif
    MO_ADD_PROPERTY_RO(QNativeGestureEvent, gestureType);
    MO_ADD_PROPERTY_RO(QNativeGestureEvent, value);
    MO_ADD_PROPERTY_RO(QNativeGestureEvent, localPos);
    MO_ADD_PROPERTY_RO(QNativeGestureEvent, windowPos);
    MO_ADD_PROPERTY_RO(QNativeGestureEvent, screenPos);
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    MO_ADD_PROPERTY_RO(QNativeGestureEvent, device);
#endif

    MO_ADD_METAOBJECT1(QKeyEvent, QInputEvent);
    MO_ADD_PROPERTY_RO(QKeyEvent, key);
    MO_ADD_PROPERTY_RO(QKeyEvent, modifiers);
    MO_ADD_PROPERTY_RO(QKeyEvent, text);
    MO_ADD_PROPERTY_RO(QKeyEvent, isAutoRepeat);
    MO_ADD_PROPERTY_RO(QKeyEvent, count);
    MO_ADD_PROPERTY_RO(QKeyEvent, nativeScanCode);
    MO_ADD_PROPERTY_RO(QKeyEvent, nativeVirtualKey);
    MO_ADD_PROPERTY_RO(QKeyEvent, nativeModifiers);

    MO_ADD_METAOBJECT1(QFocusEvent, QEvent);
    MO_ADD_PROPERTY_RO(QFocusEvent, gotFocus);
    MO_ADD_PROPERTY_RO(QFocusEvent, lostFocus);
    MO_ADD_PROPERTY_RO(QFocusEvent, reason);

    MO_ADD_METAOBJECT1(QPaintEvent, QEvent);
    MO_ADD_PROPERTY_RO(QPaintEvent, rect);
    MO_ADD_PROPERTY_RO(QPaintEvent, region);

    MO_ADD_METAOBJECT1(QInputMethodEvent, QEvent);
    MO_ADD_PROPERTY_RO(QInputMethodEvent, attributes);
    MO_ADD_PROPERTY_RO(QInputMethodEvent, preeditString);
    MO_ADD_PROPERTY_RO(QInputMethodEvent, commitString);
    MO_ADD_PROPERTY_RO(QInputMethodEvent, replacementStart);
    MO_ADD_PROPERTY_RO(QInputMethodEvent, replacementLength);

    MO_ADD_METAOBJECT1(QInputMethodQueryEvent, QEvent);
    MO_ADD_PROPERTY_RO(QInputMethodQueryEvent, queries);

    MO_ADD_METAOBJECT1(QHelpEvent, QEvent)
    MO_ADD_PROPERTY_RO(QHelpEvent, globalPos)
    MO_ADD_PROPERTY_RO(QHelpEvent, pos)

    MO_ADD_METAOBJECT1(QMoveEvent, QEvent);
    MO_ADD_PROPERTY_RO(QMoveEvent, pos);
    MO_ADD_PROPERTY_RO(QMoveEvent, oldPos);

    MO_ADD_METAOBJECT1(QExposeEvent, QEvent);
    MO_ADD_PROPERTY_RO(QExposeEvent, region);

    MO_ADD_METAOBJECT1(QResizeEvent, QEvent);
    MO_ADD_PROPERTY_RO(QResizeEvent, size);
    MO_ADD_PROPERTY_RO(QResizeEvent, oldSize);

    MO_ADD_METAOBJECT1(QWindowStateChangeEvent, QEvent);
    MO_ADD_PROPERTY_RO(QWindowStateChangeEvent, oldState);
    MO_ADD_PROPERTY_RO(QWindowStateChangeEvent, isOverride);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    MO_ADD_METAOBJECT1(QTouchEvent, QInputEvent);
    MO_ADD_PROPERTY_RO(QTouchEvent, device);
    MO_ADD_PROPERTY_RO(QTouchEvent, window);
#else
    MO_ADD_METAOBJECT1(QTouchEvent, QPointerEvent);
#endif
    MO_ADD_PROPERTY_RO(QTouchEvent, target);
    MO_ADD_PROPERTY_RO(QTouchEvent, touchPoints);

#ifndef QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    MO_ADD_METAOBJECT0(QTouchEvent::TouchPoint);
    MO_ADD_PROPERTY_RO(QTouchEvent::TouchPoint, id);
    MO_ADD_PROPERTY_RO(QTouchEvent::TouchPoint, state);
    MO_ADD_PROPERTY_RO(QTouchEvent::TouchPoint, pos);
    MO_ADD_PROPERTY_RO(QTouchEvent::TouchPoint, startPos);
    MO_ADD_PROPERTY_RO(QTouchEvent::TouchPoint, lastPos);
    // TODO: add other pos values?
    MO_ADD_PROPERTY_RO(QTouchEvent::TouchPoint, pressure);
    MO_ADD_PROPERTY_RO(QTouchEvent::TouchPoint, velocity);
    MO_ADD_PROPERTY_RO(QTouchEvent::TouchPoint, flags);
    MO_ADD_PROPERTY_RO(QTouchEvent::TouchPoint, rawScreenPositions);
#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
    MO_ADD_PROPERTY_RO(QTouchEvent::TouchPoint, rect);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    MO_ADD_PROPERTY_RO(QTouchEvent::TouchPoint, uniqueId);
    MO_ADD_PROPERTY_RO(QTouchEvent::TouchPoint, rotation);
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 0)
    MO_ADD_PROPERTY_RO(QTouchEvent::TouchPoint, ellipseDiameters);
#endif

    MO_ADD_METAOBJECT0(QTouchDevice);
    MO_ADD_PROPERTY_RO(QTouchDevice, capabilities);
    MO_ADD_PROPERTY_RO(QTouchDevice, maximumTouchPoints);
    MO_ADD_PROPERTY_RO(QTouchDevice, name);
    MO_ADD_PROPERTY_RO(QTouchDevice, type);
#endif

    MO_ADD_METAOBJECT1(QScrollPrepareEvent, QEvent);
    MO_ADD_PROPERTY_RO(QScrollPrepareEvent, startPos);
    MO_ADD_PROPERTY_RO(QScrollPrepareEvent, viewportSize);
    MO_ADD_PROPERTY_RO(QScrollPrepareEvent, contentPosRange);
    MO_ADD_PROPERTY_RO(QScrollPrepareEvent, contentPos);

    MO_ADD_METAOBJECT1(QScrollEvent, QEvent);
    MO_ADD_PROPERTY_RO(QScrollEvent, contentPos);
    MO_ADD_PROPERTY_RO(QScrollEvent, overshootDistance);
    MO_ADD_PROPERTY_RO(QScrollEvent, scrollState);

    MO_ADD_METAOBJECT1(QScreenOrientationChangeEvent, QEvent);
    MO_ADD_PROPERTY_RO(QScreenOrientationChangeEvent, screen);
    MO_ADD_PROPERTY_RO(QScreenOrientationChangeEvent, orientation);

    MO_ADD_METAOBJECT1(QShortcutEvent, QEvent);
    MO_ADD_PROPERTY_RO(QShortcutEvent, isAmbiguous);
    MO_ADD_PROPERTY_RO(QShortcutEvent, key);
    MO_ADD_PROPERTY_RO(QShortcutEvent, shortcutId);

    MO_ADD_METAOBJECT1(QStatusTipEvent, QEvent);
    MO_ADD_PROPERTY_RO(QStatusTipEvent, tip);

    MO_ADD_METAOBJECT1(QApplicationStateChangeEvent, QEvent);
    MO_ADD_PROPERTY_RO(QApplicationStateChangeEvent, applicationState);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    MO_ADD_METAOBJECT1(QEnterEvent, QEvent);
    MO_ADD_PROPERTY_RO(QEnterEvent, globalPos);
    MO_ADD_PROPERTY_RO(QEnterEvent, pos);
#else
    MO_ADD_METAOBJECT1(QEnterEvent, QSinglePointEvent);
#endif
    MO_ADD_PROPERTY_RO(QEnterEvent, localPos);
    MO_ADD_PROPERTY_RO(QEnterEvent, screenPos);
    MO_ADD_PROPERTY_RO(QEnterEvent, windowPos);
}

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

#define E(x) { QContextMenuEvent::x , #x }
static const MetaEnum::Value<QContextMenuEvent::Reason> context_menu_reason_table[] = {
    E(Mouse),
    E(Keyboard),
    E(Other)
};
#undef E

#define E(x) { QSurfaceFormat:: x, #x }
static const MetaEnum::Value<QSurfaceFormat::FormatOption> surface_format_option_table[] = {
    E(StereoBuffers),
    E(DebugContext),
    E(DeprecatedFunctions),
    E(ResetNotification)
};
#undef E

#define E(x) { QFont:: x, #x }
static const MetaEnum::Value<QFont::Capitalization> font_capitalization_table[] = {
    E(MixedCase),
    E(AllUppercase),
    E(AllLowercase),
    E(SmallCaps),
    E(Capitalize)
};

static const MetaEnum::Value<QFont::HintingPreference> font_hinting_pref_table[] = {
    E(PreferDefaultHinting),
    E(PreferNoHinting),
    E(PreferVerticalHinting),
    E(PreferFullHinting)
};

static const MetaEnum::Value<QFont::SpacingType> font_spacing_type_table[] = {
    E(PercentageSpacing),
    E(AbsoluteSpacing)
};

static const MetaEnum::Value<QFont::Style> font_style_table[] = {
    E(StyleNormal),
    E(StyleItalic),
    E(StyleOblique)
};

static const MetaEnum::Value<QFont::StyleHint> font_style_hint_table[] = {
    E(AnyStyle),
    E(SansSerif),
    E(Helvetica),
    E(Serif),
    E(Times),
    E(TypeWriter),
    E(Courier),
    E(OldEnglish),
    E(Decorative),
    E(Monospace),
    E(Fantasy),
    E(Cursive),
    E(System)
};
#undef E

#define E(x) { Qt:: x, #x }
static const MetaEnum::Value<Qt::MouseEventFlag> mouse_event_flag_table[] = {
    E(MouseEventCreatedDoubleClick),
    E(MouseEventFlagMask)
};
#undef E

#define E(x) { QPainter:: x, #x }
static const MetaEnum::Value<QPainter::CompositionMode> painter_composition_mode_table[] = {
    E(CompositionMode_SourceOver),
    E(CompositionMode_DestinationOver),
    E(CompositionMode_Clear),
    E(CompositionMode_Source),
    E(CompositionMode_Destination),
    E(CompositionMode_SourceIn),
    E(CompositionMode_DestinationIn),
    E(CompositionMode_SourceOut),
    E(CompositionMode_DestinationOut),
    E(CompositionMode_SourceAtop),
    E(CompositionMode_DestinationAtop),
    E(CompositionMode_Xor),
    E(CompositionMode_Plus),
    E(CompositionMode_Multiply),
    E(CompositionMode_Screen),
    E(CompositionMode_Overlay),
    E(CompositionMode_Darken),
    E(CompositionMode_Lighten),
    E(CompositionMode_ColorDodge),
    E(CompositionMode_ColorBurn),
    E(CompositionMode_HardLight),
    E(CompositionMode_SoftLight),
    E(CompositionMode_Difference),
    E(CompositionMode_Exclusion),
    E(RasterOp_SourceOrDestination),
    E(RasterOp_SourceAndDestination),
    E(RasterOp_SourceXorDestination),
    E(RasterOp_NotSourceAndNotDestination),
    E(RasterOp_NotSourceOrNotDestination),
    E(RasterOp_NotSourceXorDestination),
    E(RasterOp_NotSource),
    E(RasterOp_NotSourceAndDestination),
    E(RasterOp_SourceAndNotDestination),
    E(RasterOp_NotSourceOrDestination),
    E(RasterOp_ClearDestination),
    E(RasterOp_SetDestination),
    E(RasterOp_NotDestination),
    E(RasterOp_SourceOrNotDestination)
};

static const MetaEnum::Value<QPainter::RenderHint> painter_render_hint_table[] = {
    E(Antialiasing),
    E(TextAntialiasing),
    E(SmoothPixmapTransform),
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    E(HighQualityAntialiasing),
    E(NonCosmeticDefaultPen),
    E(Qt4CompatiblePainting),
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 13, 0)
    E(LosslessImageRendering),
#endif
};
#undef E

#define E(x) { QPaintEngine:: x, #x }
static const MetaEnum::Value<QPaintEngine::PolygonDrawMode> paintengine_polygon_draw_mode_table[] = {
    E(OddEvenMode),
    E(WindingMode),
    E(ConvexMode),
    E(PolylineMode)
};
#undef E

#define E(x) { QPlatformPixmap:: x, #x }
static const MetaEnum::Value<QPlatformPixmap::ClassId> platformpixmap_classid_table[] = {
    E(RasterClass),
    E(DirectFBClass),
    E(BlitterClass),
    E(Direct2DClass),
    E(CustomClass)
};
#undef E

#define E(x) { QImage:: x, #x }
static const MetaEnum::Value<QImage::Format> image_format_table[] = {
    E(Format_Invalid),
    E(Format_Mono),
    E(Format_MonoLSB),
    E(Format_Indexed8),
    E(Format_RGB32),
    E(Format_ARGB32),
    E(Format_ARGB32_Premultiplied),
    E(Format_RGB16),
    E(Format_ARGB8565_Premultiplied),
    E(Format_RGB666),
    E(Format_ARGB6666_Premultiplied),
    E(Format_RGB555),
    E(Format_ARGB8555_Premultiplied),
    E(Format_RGB888),
    E(Format_RGB444),
    E(Format_ARGB4444_Premultiplied),
    E(Format_RGBX8888),
    E(Format_RGBA8888),
    E(Format_RGBA8888_Premultiplied),
    E(Format_BGR30),
    E(Format_A2BGR30_Premultiplied),
    E(Format_RGB30),
    E(Format_A2RGB30_Premultiplied),
    E(Format_Alpha8),
    E(Format_Grayscale8)
};
#undef E

#define E(x) { QPixelFormat:: x, #x }
static const MetaEnum::Value<QPixelFormat::AlphaPosition> pixelformat_alphaposition_table[] = {
    E(AtBeginning),
    E(AtEnd)
};

static const MetaEnum::Value<QPixelFormat::AlphaPremultiplied> pixelformat_alphapremultiplied_table[] = {
    E(NotPremultiplied),
    E(Premultiplied)
};

static const MetaEnum::Value<QPixelFormat::AlphaUsage> pixelformat_alphausage_table[] = {
    E(UsesAlpha),
    E(IgnoresAlpha)
};

static const MetaEnum::Value<QPixelFormat::ByteOrder> pixelformat_byteorder_table[] = {
    E(LittleEndian),
    E(BigEndian),
    E(CurrentSystemEndian)
};

static const MetaEnum::Value<QPixelFormat::ColorModel> pixelformat_colormodel_table[] = {
    E(RGB),
    E(BGR),
    E(Indexed),
    E(Grayscale),
    E(CMYK),
    E(HSL),
    E(HSV),
    E(YUV),
    E(Alpha)
};

static const MetaEnum::Value<QPixelFormat::TypeInterpretation> pixelformat_typeinterpretation_table[] = {
    E(UnsignedInteger),
    E(UnsignedShort),
    E(UnsignedByte),
    E(FloatingPoint)
};

static const MetaEnum::Value<QPixelFormat::YUVLayout> pixelformat_yuvlayout_table[] = {
    E(YUV444),
    E(YUV422),
    E(YUV411),
    E(YUV420P),
    E(YUV420SP),
    E(YV12),
    E(UYVY),
    E(YUYV),
    E(NV12),
    E(NV21),
    E(IMC1),
    E(IMC2),
    E(IMC3),
    E(IMC4),
    E(Y8),
    E(Y16)
};
#undef E

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
#define E(x) { QTouchEvent::TouchPoint:: x, #x }
static const MetaEnum::Value<QTouchEvent::TouchPoint::InfoFlags> touch_point_info_flag_table[] = {
    E(Pen),
#if QT_VERSION >= QT_VERSION_CHECK(5, 8, 0)
    E(Token)
#endif
};
#undef E

#define E(x) { QTouchDevice:: x, #x }
static const MetaEnum::Value<QTouchDevice::Capabilities> touch_device_capabilitites_flag_table[] = {
    E(Position),
    E(Area),
    E(Pressure),
    E(Velocity),
    E(RawPositions),
    E(NormalizedPosition),
    E(MouseEmulation)
};
#undef E
#endif

static QString brushToString(const QBrush &b)
{
    return VariantHandler::displayString(b.color()) + QLatin1String(", ") + EnumUtil::enumToString(QVariant::fromValue(b.style()));
}

static QString penToString(const QPen &p)
{
    QStringList l;
    l.reserve(8);
    l.push_back(GuiSupport::tr("width: %1").arg(p.width()));
    l.push_back(GuiSupport::tr("brush: %1").arg(brushToString(p.brush())));
    l.push_back(EnumUtil::enumToString(QVariant::fromValue(p.style())));
    l.push_back(EnumUtil::enumToString(QVariant::fromValue(p.capStyle())));
    l.push_back(EnumUtil::enumToString(QVariant::fromValue(p.joinStyle())));
    if (p.joinStyle() == Qt::MiterJoin)
        l.push_back(GuiSupport::tr("miter limit: %1").arg(p.miterLimit()));
    if (!p.dashPattern().isEmpty()) {
        QStringList dashes;
        dashes.reserve(p.dashPattern().size());
        foreach (auto v, p.dashPattern())
            dashes.push_back(QString::number(v));
        l.push_back(GuiSupport::tr("dash pattern: (%1)").arg(dashes.join(QLatin1String(", "))));
    }
    if (p.dashOffset() != 0.0)
        l.push_back(GuiSupport::tr("dash offset: %1").arg(p.dashOffset()));
    return l.join(QLatin1String(", "));
}

static QString regionToString(const QRegion &region)
{
    if (region.isNull())
        return QStringLiteral("<null>");
    if (region.isEmpty())
        return QStringLiteral("<empty>");
#if QT_VERSION > QT_VERSION_CHECK(5, 8, 0)
    if (region.rectCount() == 1)
        return VariantHandler::displayString(*region.begin());

    QStringList rects;
    rects.reserve(region.rectCount());
    for (const auto &r :  region)
        rects.push_back(VariantHandler::displayString(r));

    return GuiSupport::tr("[%1]: %2").arg(
        VariantHandler::displayString(region.boundingRect()),
        rects.join(QLatin1String("; "))
    );
#else
    return GuiSupport::tr("<%1 elements>").arg(region.rectCount());
#endif
}

static QString imageToString(const QImage &image)
{
    return VariantHandler::displayString(image.size());
}

static QString pixmapToString(const QPixmap &pixmap)
{
    return VariantHandler::displayString(pixmap.size());
}

void GuiSupport::registerVariantHandler()
{
    VariantHandler::registerStringConverter<const QValidator*>(Util::displayString);
    VariantHandler::registerStringConverter<const QMimeData*>(Util::displayString);
    VariantHandler::registerStringConverter<QSurfaceFormat>(surfaceFormatToString);

    ER_REGISTER_ENUM(QContextMenuEvent, Reason, context_menu_reason_table);
    ER_REGISTER_ENUM(QSurface, SurfaceClass, surface_class_table);
    ER_REGISTER_ENUM(QSurface, SurfaceType, surface_type_table);
    ER_REGISTER_FLAGS(QSurfaceFormat, FormatOptions, surface_format_option_table);

    ER_REGISTER_ENUM(QFont, Capitalization, font_capitalization_table);
    ER_REGISTER_ENUM(QFont, HintingPreference, font_hinting_pref_table);
    ER_REGISTER_ENUM(QFont, SpacingType, font_spacing_type_table);
    ER_REGISTER_ENUM(QFont, Style, font_style_table);
    ER_REGISTER_ENUM(QFont, StyleHint, font_style_hint_table);

    ER_REGISTER_ENUM(QImage, Format, image_format_table);
    ER_REGISTER_ENUM(QPainter, CompositionMode, painter_composition_mode_table);
    ER_REGISTER_FLAGS(QPainter, RenderHints, painter_render_hint_table);
    ER_REGISTER_ENUM(QPaintEngine, PolygonDrawMode, paintengine_polygon_draw_mode_table);

    ER_REGISTER_FLAGS(Qt, MouseEventFlags, mouse_event_flag_table);

    VariantHandler::registerStringConverter<QBrush>(brushToString);
    VariantHandler::registerStringConverter<const QGradient*>(Util::addressToString);
    VariantHandler::registerStringConverter<QImage>(imageToString);
    VariantHandler::registerStringConverter<QPainterPath>(painterPathToString);
    VariantHandler::registerStringConverter<QPen>(penToString);
    VariantHandler::registerStringConverter<QPixmap>(pixmapToString);
    VariantHandler::registerStringConverter<QRegion>(regionToString);
    VariantHandler::registerStringConverter<QTextLength>(textLengthToString);
    VariantHandler::registerStringConverter<QPair<double, QColor> >([](const QPair<double, QColor> &p) {
        return QString(VariantHandler::displayString(p.first) + QLatin1String(": ") + VariantHandler::displayString(p.second));
    });
    ER_REGISTER_ENUM(QPlatformPixmap, ClassId, platformpixmap_classid_table);
    VariantHandler::registerStringConverter<QImage*>(Util::addressToString);
    VariantHandler::registerStringConverter<QPlatformPixmap*>(Util::addressToString);

    ER_REGISTER_ENUM(QPixelFormat, AlphaPosition, pixelformat_alphaposition_table);
    ER_REGISTER_ENUM(QPixelFormat, AlphaPremultiplied, pixelformat_alphapremultiplied_table);
    ER_REGISTER_ENUM(QPixelFormat, AlphaUsage, pixelformat_alphausage_table);
    ER_REGISTER_ENUM(QPixelFormat, ByteOrder, pixelformat_byteorder_table);
    ER_REGISTER_ENUM(QPixelFormat, ColorModel, pixelformat_colormodel_table);
    ER_REGISTER_ENUM(QPixelFormat, TypeInterpretation, pixelformat_typeinterpretation_table);
    ER_REGISTER_ENUM(QPixelFormat, YUVLayout, pixelformat_yuvlayout_table);

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    ER_REGISTER_FLAGS(QTouchEvent::TouchPoint, InfoFlags, touch_point_info_flag_table);
    ER_REGISTER_FLAGS(QTouchDevice, Capabilities, touch_device_capabilitites_flag_table);
#endif
}

QObject *GuiSupport::targetObject(QObject *object) const
{
    return object ? object : qobject_cast<QObject *>(qApp);
}

QIcon GuiSupport::createIcon(const QIcon &oldIcon, QWindow *w)
{
    static QIcon gammarayIcon;
    if (gammarayIcon.availableSizes().isEmpty()) {
        gammarayIcon.addFile(QLatin1String(":/gammaray/images/gammaray-inject-16.png"));
        gammarayIcon.addFile(QLatin1String(":/gammaray/images/gammaray-inject-22.png"));
        gammarayIcon.addFile(QLatin1String(":/gammaray/images/gammaray-inject-24.png"));
        gammarayIcon.addFile(QLatin1String(":/gammaray/images/gammaray-inject-32.png"));
        gammarayIcon.addFile(QLatin1String(":/gammaray/images/gammaray-inject-48.png"));
        gammarayIcon.addFile(QLatin1String(":/gammaray/images/gammaray-inject-64.png"));
        gammarayIcon.addFile(QLatin1String(":/gammaray/images/gammaray-inject-128.png"));
    }

    QObject *target = targetObject(w);

    // As windows icon can be provided by the qXXApplication::windowIcon()
    // We need to compute this one first.
    if (target != qApp) {
        // There is no real notification to track application window icon change.
        // Then make sure the application icon is uptodate first.
        const auto ait = m_iconAndTitleOverrider.objectsIcons.find(qApp);

        if (ait != m_iconAndTitleOverrider.objectsIcons.end()) {
            // The application window icon changed... rebuild it.
            if (ait.value().gammarayIcon.cacheKey() != qApp->windowIcon().cacheKey()) {
                m_iconAndTitleOverrider.objectsIcons.erase(ait);
                m_iconAndTitleOverrider.updatingObjectsIcon.remove(w);
                updateWindowIcon();
                m_iconAndTitleOverrider.updatingObjectsIcon << w;
                return oldIcon;
            }
        } else {
            // Build the application icon
            m_iconAndTitleOverrider.updatingObjectsIcon.remove(w);
            updateWindowIcon();
            m_iconAndTitleOverrider.updatingObjectsIcon << w;
            return oldIcon;
        }
    }

    const auto it = m_iconAndTitleOverrider.objectsIcons.constFind(target);
    if (it != m_iconAndTitleOverrider.objectsIcons.constEnd() && it.value().gammarayIcon.cacheKey() == oldIcon.cacheKey()) {
        return oldIcon;
    }

    const bool highDpiEnabled = qApp->testAttribute(Qt::AA_UseHighDpiPixmaps);
    QIcon newIcon;
    foreach (const QSize &size, gammarayIcon.availableSizes()) {
        QPixmap pix = oldIcon.pixmap(oldIcon.actualSize(size));
        if (pix.isNull()) {
            const qreal ratio = highDpiEnabled ? (w ? w->devicePixelRatio() : qApp->devicePixelRatio()) : 1.0;
            pix = QPixmap(size * ratio);
            pix.setDevicePixelRatio(ratio);
            pix.fill(Qt::transparent);
        }
        {
            QPainter p(&pix);
            gammarayIcon.paint(&p, QRect(QPoint(), pix.size() / pix.devicePixelRatio()));
        }
        newIcon.addPixmap(pix);
    }
    return newIcon;
}

void GuiSupport::updateWindowIcon(QWindow *w)
{
    QObject *target = targetObject(w);
    Q_ASSERT(!m_iconAndTitleOverrider.updatingObjectsIcon.contains(target));
    m_iconAndTitleOverrider.updatingObjectsIcon << target;

    const QIcon oldIcon = w ? w->icon() : qApp->windowIcon();
    const QIcon newIcon = createIcon(oldIcon, w);

    if (oldIcon.cacheKey() != newIcon.cacheKey()) {
        m_iconAndTitleOverrider.objectsIcons.insert(target,
                                                    IconAndTitleOverriderData::Icons(oldIcon, newIcon));
        if (w)
            w->setIcon(newIcon);
        else
            qApp->setWindowIcon(newIcon);
    }

    m_iconAndTitleOverrider.updatingObjectsIcon.remove(target);

    if (!w && m_iconAndTitleOverrider.updatingObjectsIcon.isEmpty()) {
        foreach (auto w, qApp->topLevelWindows()) {
            if (isAcceptableWindow(w))
                updateWindowIcon(w);
        }
    }
}

void GuiSupport::updateWindowTitle(QWindow *w)
{
    QObject *target = targetObject(w);
    Q_ASSERT(!m_iconAndTitleOverrider.updatingObjectsTitle.contains(target));
    m_iconAndTitleOverrider.updatingObjectsTitle << target;

    if (!w->title().endsWith(m_iconAndTitleOverrider.titleSuffix))
        w->setTitle(w->title() + m_iconAndTitleOverrider.titleSuffix);

    m_iconAndTitleOverrider.updatingObjectsTitle.remove(target);
}

void GuiSupport::restoreWindowIcon(QWindow *w)
{
    QObject *target = targetObject(w);
    Q_ASSERT(!m_iconAndTitleOverrider.updatingObjectsIcon.contains(target));
    m_iconAndTitleOverrider.updatingObjectsIcon << target;

    auto it = m_iconAndTitleOverrider.objectsIcons.find(target);
    if (it != m_iconAndTitleOverrider.objectsIcons.end()) {
        if (w)
            w->setIcon(it.value().originalIcon);
        else
            qApp->setWindowIcon(it.value().originalIcon);
        m_iconAndTitleOverrider.objectsIcons.erase(it);
    }

    m_iconAndTitleOverrider.updatingObjectsIcon.remove(target);

    if (!w && m_iconAndTitleOverrider.updatingObjectsIcon.isEmpty()) {
        foreach (auto w, qApp->topLevelWindows()) {
            if (isAcceptableWindow(w))
                restoreWindowIcon(w);
        }
    }
}

void GuiSupport::restoreWindowTitle(QWindow *w)
{
    Q_ASSERT(!m_iconAndTitleOverrider.updatingObjectsTitle.contains(w));
    m_iconAndTitleOverrider.updatingObjectsTitle << w;

    w->setTitle(w->title().remove(m_iconAndTitleOverrider.titleSuffix));

    m_iconAndTitleOverrider.updatingObjectsTitle.remove(w);
}

void GuiSupport::restoreIconAndTitle()
{
    if (qApp->closingDown())
        return;

    restoreWindowIcon();
    foreach (auto w, qApp->topLevelWindows()) {
        if (isAcceptableWindow(w))
            restoreWindowTitle(w);
    }
}

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

bool GuiSupport::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::WindowIconChange) {
        if (auto w = qobject_cast<QWindow*>(watched)) {
            if (!m_iconAndTitleOverrider.updatingObjectsIcon.contains(qApp)
                    && !m_iconAndTitleOverrider.updatingObjectsIcon.contains(w)) {
                if (isAcceptableWindow(w))
                    updateWindowIcon(w);
            }
        }
    } else if (event->type() == QEvent::WindowTitleChange) {
        if (auto w = qobject_cast<QWindow*>(watched)) {
            if (!m_iconAndTitleOverrider.updatingObjectsTitle.contains(qApp)
                    && !m_iconAndTitleOverrider.updatingObjectsTitle.contains(w)) {
                if (isAcceptableWindow(w))
                    updateWindowTitle(w);
            }
        }
    }

    return QObject::eventFilter(watched, event);
}

GuiSupportFactory::GuiSupportFactory(QObject *parent)
    : QObject(parent)
{
}
