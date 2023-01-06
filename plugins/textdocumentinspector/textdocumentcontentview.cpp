/*
  textdocumentcontentview.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include "textdocumentcontentview.h"

#include <QPainter>

using namespace GammaRay;

TextDocumentContentView::TextDocumentContentView(QWidget *parent)
    : QTextEdit(parent)
{
}

void TextDocumentContentView::setShowBoundingBox(const QRectF &boundingBox)
{
    m_boundingBox = boundingBox;
    viewport()->update();
}

void TextDocumentContentView::paintEvent(QPaintEvent *e)
{
    QTextEdit::paintEvent(e);
    if (!m_boundingBox.isEmpty()) {
        QPainter painter(viewport());
        painter.setPen(Qt::red);
        painter.drawRect(m_boundingBox);
    }
}
