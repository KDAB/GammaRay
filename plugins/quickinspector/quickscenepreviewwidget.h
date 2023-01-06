/*
  quickscenepreviewwidget.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Anton Kreuzkamp <anton.kreuzkamp@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKSCENEPREVIEWWIDGET_H
#define GAMMARAY_QUICKINSPECTOR_QUICKSCENEPREVIEWWIDGET_H

#include "quickdecorationsdrawer.h"
#include "quickitemgeometry.h"
#include "quickinspectorinterface.h"
#include "quickscenecontrolwidget.h"

#include <ui/remoteviewwidget.h>

namespace GammaRay {
class QuickInspectorInterface;

struct CompleteFrameRequest
{
    CompleteFrameRequest(const QString &filePath = QString(), bool drawDecoration = false)
        : filePath(filePath)
        , drawDecoration(drawDecoration)
    {
    }

    bool isValid() const
    {
        return !filePath.isEmpty();
    }

    void reset()
    {
        filePath.clear();
        drawDecoration = false;
    }

    CompleteFrameRequest &operator=(const CompleteFrameRequest &other)
    {
        if (&other != this) {
            filePath = other.filePath;
            drawDecoration = other.drawDecoration;
        }

        return *this;
    }

    QString filePath;
    bool drawDecoration;
};

class QuickScenePreviewWidget : public RemoteViewWidget
{
    Q_OBJECT

public:
    explicit QuickScenePreviewWidget(QuickSceneControlWidget *control, QWidget *parent = nullptr);
    ~QuickScenePreviewWidget() override;

    Q_INVOKABLE void restoreState(const QByteArray &state) override;
    QByteArray saveState() const override;

    QuickDecorationsSettings overlaySettings() const;
    void setOverlaySettings(const QuickDecorationsSettings &settings);

    void requestCompleteFrame(const CompleteFrameRequest &request);

private slots:
    void saveScreenshot();

private:
    void drawDecoration(QPainter *p) override;
    void resizeEvent(QResizeEvent *e) override;

    void renderDecoration(QPainter *p, double zoom) const;

    QuickSceneControlWidget *m_control;
    QuickDecorationsSettings m_overlaySettings;
    CompleteFrameRequest m_pendingCompleteFrame;
};
} // namespace GammaRay

#endif
