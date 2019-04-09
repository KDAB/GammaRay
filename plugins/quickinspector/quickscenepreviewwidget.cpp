/*
  quickscenepreviewwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

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

#include "quickscenepreviewwidget.h"
#include "quickinspectorinterface.h"

#include <common/remoteviewinterface.h>
#include <common/streamoperators.h>

#include <QMouseEvent>
#include <QPainter>

using namespace GammaRay;
static const qint32 QuickScenePreviewWidgetStateVersion = 4;

QT_BEGIN_NAMESPACE
GAMMARAY_ENUM_STREAM_OPERATORS(GammaRay::QuickInspectorInterface::RenderMode)
QT_END_NAMESPACE

QuickScenePreviewWidget::QuickScenePreviewWidget(QuickSceneControlWidget *control,
                                                 QWidget *parent)
    : RemoteViewWidget(parent)
    , m_control(control)
{
    connect(this, &RemoteViewWidget::frameChanged, this, &QuickScenePreviewWidget::saveScreenshot);
    setName(QStringLiteral("com.kdab.GammaRay.QuickRemoteView"));
    setUnavailableText(tr("No remote view available.\n(This happens e.g. when the window is minimized or the scene is hidden)"));
}

QuickScenePreviewWidget::~QuickScenePreviewWidget() = default;

void QuickScenePreviewWidget::restoreState(const QByteArray &state)
{
    if (state.isEmpty())
        return;

    QDataStream stream(state);
    qint32 version;
    QuickInspectorInterface::RenderMode mode = m_control->customRenderMode();
    bool drawDecorations = m_control->serverSideDecorationsEnabled();
    QPointF gridOffset = m_overlaySettings.gridOffset;
    QSizeF gridCellSize = m_overlaySettings.gridCellSize;
    bool gridEnabled = m_overlaySettings.gridEnabled;
    RemoteViewWidget::restoreState(stream);

    stream >> version;

    switch (version) {
    case 1: {
        stream
                >> mode
                ;
        break;
    }
    case 2: {
        stream
                >> mode
                >> drawDecorations
                ;
        break;
    }
    case 3: {
        stream
                >> mode
                >> drawDecorations
                >> gridOffset
                >> gridCellSize
                ;
        break;
    }
    case QuickScenePreviewWidgetStateVersion: {
        stream
                >> mode
                >> drawDecorations
                >> gridOffset
                >> gridCellSize
                >> gridEnabled
                ;
        break;
    }
    }

    m_control->setCustomRenderMode(mode);
    m_control->setServerSideDecorationsEnabled(drawDecorations);

    QuickDecorationsSettings settings = m_overlaySettings;
    settings.gridOffset = gridOffset;
    settings.gridCellSize = gridCellSize;
    settings.componentsTraces = (mode == QuickInspectorInterface::VisualizeTraces);
    settings.gridEnabled = gridEnabled;

    if (settings != m_overlaySettings) {
        m_control->setOverlaySettings(settings);
    }
}

QByteArray QuickScenePreviewWidget::saveState() const
{
    QByteArray data;

    {
        QDataStream stream(&data, QIODevice::WriteOnly);
        RemoteViewWidget::saveState(stream);

        stream << QuickScenePreviewWidgetStateVersion;

        switch (QuickScenePreviewWidgetStateVersion) {
        case 1: {
            stream
                    << m_control->customRenderMode()
                       ;
            break;
        }
        case 2: {
            stream
                    << m_control->customRenderMode()
                    << m_control->serverSideDecorationsEnabled()
                       ;
            break;
        }
        case 3: {
            stream
                    << m_control->customRenderMode()
                    << m_control->serverSideDecorationsEnabled()
                    << m_overlaySettings.gridOffset
                    << m_overlaySettings.gridCellSize
                       ;
            break;
        }
        case 4: {
            stream
                    << m_control->customRenderMode()
                    << m_control->serverSideDecorationsEnabled()
                    << m_overlaySettings.gridOffset
                    << m_overlaySettings.gridCellSize
                    << m_overlaySettings.gridEnabled
                       ;
            break;
        }
        }
    }

    return data;
}

void QuickScenePreviewWidget::resizeEvent(QResizeEvent *e)
{
    RemoteViewWidget::resizeEvent(e);
}

void QuickScenePreviewWidget::renderDecoration(QPainter *p, double zoom) const
{
    // Scaling and translations on QuickItemGeometry will be done on demand

    if (frame().data().userType() == qMetaTypeId<QuickItemGeometry>()) {
        // scaled and translated
        const auto itemGeometry = frame().data().value<QuickItemGeometry>();
        const QuickDecorationsRenderInfo renderInfo(m_overlaySettings, itemGeometry, frame().viewRect(), zoom);
        QuickDecorationsDrawer drawer(QuickDecorationsDrawer::Decorations, *p, renderInfo);
        drawer.render();
    } else if (frame().data().userType() == qMetaTypeId<QVector<QuickItemGeometry>>()) {
        const auto itemsGeometry = frame().data().value<QVector<QuickItemGeometry>>();
        const QuickDecorationsTracesInfo tracesInfo(m_overlaySettings, itemsGeometry, frame().viewRect(), zoom);
        QuickDecorationsDrawer drawer(QuickDecorationsDrawer::Traces, *p, tracesInfo);
        drawer.render();
    }
}

void QuickScenePreviewWidget::drawDecoration(QPainter *p)
{
    renderDecoration(p, zoom());
}

QuickDecorationsSettings QuickScenePreviewWidget::overlaySettings() const
{
    return m_overlaySettings;
}

void QuickScenePreviewWidget::setOverlaySettings(const QuickDecorationsSettings &settings)
{
    m_overlaySettings = settings;
    update();
}

void QuickScenePreviewWidget::requestCompleteFrame(const CompleteFrameRequest &request)
{
    if (m_pendingCompleteFrame.isValid()) {
        qWarning("A pending complete frame request is already running.");
        return;
    }

    m_pendingCompleteFrame = request;
    remoteViewInterface()->requestCompleteFrame();
}

void QuickScenePreviewWidget::saveScreenshot()
{
    if (!m_pendingCompleteFrame.isValid() || !hasValidCompleteFrame())
        return;

    const QImage &source(frame().image());
    QImage img(source.size(), source.format());
    img.setDevicePixelRatio(source.devicePixelRatio());
    QPainter p(&img);

    p.save();
    p.setTransform(frame().transform(), true);
    p.drawImage(QPoint(), source);
    p.restore();
    if (m_pendingCompleteFrame.drawDecoration)
        renderDecoration(&p, 1.0);
    p.end();

    img.save(m_pendingCompleteFrame.filePath);

    m_pendingCompleteFrame.reset();
}
