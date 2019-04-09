/*
  quickscenepreviewwidget.h

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

#ifndef GAMMARAY_QUICKINSPECTOR_QUICKSCENEPREVIEWWIDGET_H
#define GAMMARAY_QUICKINSPECTOR_QUICKSCENEPREVIEWWIDGET_H

#include "quickdecorationsdrawer.h"
#include "quickitemgeometry.h"
#include "quickinspectorinterface.h"
#include "quickscenecontrolwidget.h"

#include <ui/remoteviewwidget.h>

namespace GammaRay {
class QuickInspectorInterface;

struct CompleteFrameRequest {
    CompleteFrameRequest(const QString &filePath = QString(), bool drawDecoration = false)
        : filePath(filePath)
        , drawDecoration(drawDecoration)
    { }

    bool isValid() const
    { return !filePath.isEmpty(); }

    void reset()
    { filePath.clear(); drawDecoration = false; }

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
