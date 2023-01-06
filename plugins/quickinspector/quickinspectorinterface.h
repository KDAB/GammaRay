/*
  quickinspectorinterface.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKINSPECTORINTERFACE_H
#define GAMMARAY_QUICKINSPECTOR_QUICKINSPECTORINTERFACE_H

#include <common/streamoperators.h>

#include "quickitemgeometry.h"

#include <QObject>
#include <QRectF>
#include <QVariantMap>
#include <QEvent>

QT_BEGIN_NAMESPACE
class QImage;
QT_END_NAMESPACE

namespace GammaRay {
struct QuickDecorationsSettings;

class QuickInspectorInterface : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool serverSideDecoration READ serverSideDecorationEnabled WRITE setServerSideDecorationsEnabled NOTIFY serverSideDecorationChanged)
public:
    enum Feature
    {
        NoFeatures = 0,
        CustomRenderModeClipping = 1,
        CustomRenderModeOverdraw = 2,
        CustomRenderModeBatches = 4,
        CustomRenderModeChanges = 8,
        CustomRenderModeTraces = 16,
        AllCustomRenderModes = CustomRenderModeClipping | CustomRenderModeOverdraw
            | CustomRenderModeBatches | CustomRenderModeChanges
            | CustomRenderModeTraces,
        AnalyzePainting
    };

    enum RenderMode
    {
        NormalRendering,
        VisualizeClipping,
        VisualizeOverdraw,
        VisualizeBatches,
        VisualizeChanges,
        VisualizeTraces,
    };

    Q_ENUMS(RenderMode)
    Q_DECLARE_FLAGS(Features, Feature)

    explicit QuickInspectorInterface(QObject *parent = nullptr);
    ~QuickInspectorInterface() override;

    bool serverSideDecorationEnabled() const;
    void setServerSideDecorationsEnabled(bool enabled);

public slots:
    virtual void selectWindow(int index) = 0;

    virtual void setCustomRenderMode(
        GammaRay::QuickInspectorInterface::RenderMode customRenderMode) = 0;

    virtual void checkFeatures() = 0;

    virtual void setOverlaySettings(const GammaRay::QuickDecorationsSettings &settings) = 0;

    virtual void checkOverlaySettings() = 0;

    virtual void analyzePainting() = 0;

    virtual void checkSlowMode() = 0;
    virtual void setSlowMode(bool slow) = 0;

signals:
    void features(GammaRay::QuickInspectorInterface::Features features);
    void serverSideDecorationChanged(bool enabled);
    void overlaySettings(const GammaRay::QuickDecorationsSettings &settings);
    void slowModeChanged(bool slow);

private:
    bool m_serverSideDecoration;
};
}

Q_DECLARE_METATYPE(GammaRay::QuickInspectorInterface::Features)
Q_DECLARE_METATYPE(GammaRay::QuickInspectorInterface::RenderMode)
QT_BEGIN_NAMESPACE
Q_DECLARE_INTERFACE(GammaRay::QuickInspectorInterface,
                    "com.kdab.GammaRay.QuickInspectorInterface/1.0")
QT_END_NAMESPACE

#endif // GAMMARAY_QUICKINSPECTORINTERFACE_H
