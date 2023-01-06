/*
  paintanalyzerreplayview.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "paintanalyzerreplayview.h"

#include <common/paintanalyzerinterface.h>

#include <QDebug>
#include <QPainter>

using namespace GammaRay;

PaintAnalyzerReplayView::PaintAnalyzerReplayView(QWidget *parent)
    : RemoteViewWidget(parent)
    , m_showClipArea(true)
{
}

PaintAnalyzerReplayView::~PaintAnalyzerReplayView() = default;

bool PaintAnalyzerReplayView::showClipArea() const
{
    return m_showClipArea;
}

void PaintAnalyzerReplayView::setShowClipArea(bool show)
{
    m_showClipArea = show;
    update();
}

void PaintAnalyzerReplayView::drawDecoration(QPainter *p)
{
    const auto data = frame().data().value<PaintAnalyzerFrameData>();
    if (data.clipPath.isEmpty() || !m_showClipArea)
        return;

    QPainterPath invertedClipPath;
    invertedClipPath.addRect(frame().sceneRect());
    invertedClipPath -= data.clipPath;

    p->save();
    p->setTransform(QTransform().scale(zoom(), zoom()), true);
    QBrush brush(Qt::red, Qt::BDiagPattern);
    p->fillPath(invertedClipPath, brush);
    p->restore();
}
