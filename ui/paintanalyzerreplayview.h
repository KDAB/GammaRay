/*
  paintanalyzerreplayview.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PAINTANALYZERREPLAYVIEW_H
#define GAMMARAY_PAINTANALYZERREPLAYVIEW_H

#include "remoteviewwidget.h"

namespace GammaRay {

class PaintAnalyzerReplayView : public RemoteViewWidget
{
    Q_OBJECT
public:
    explicit PaintAnalyzerReplayView(QWidget *parent = nullptr);
    ~PaintAnalyzerReplayView() override;

    bool showClipArea() const;

public slots:
    void setShowClipArea(bool show);

protected:
    void drawDecoration(QPainter *p) override;

private:
    bool m_showClipArea;
};
}

#endif // GAMMARAY_PAINTANALYZERREPLAYVIEW_H
