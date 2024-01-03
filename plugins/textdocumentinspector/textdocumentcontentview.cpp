/*
  textdocumentcontentview.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
