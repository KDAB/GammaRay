/*
  codeeditorsidebar.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "codeeditorsidebar.h"
#include "codeeditor.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QTextBlock>

using namespace GammaRay;

CodeEditorSidebar::CodeEditorSidebar(CodeEditor *editor)
    : QWidget(editor)
    , m_codeEditor(editor)
{
}

CodeEditorSidebar::~CodeEditorSidebar() = default;

QSize CodeEditorSidebar::sizeHint() const
{
    return { m_codeEditor->sidebarWidth(), 0 };
}

void CodeEditorSidebar::paintEvent(QPaintEvent *event)
{
    m_codeEditor->sidebarPaintEvent(event);
}

void CodeEditorSidebar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->pos().x() >= width() - m_codeEditor->foldingBarWidth()) {
        auto block = m_codeEditor->blockAtPosition(event->pos().y());
        if (!block.isValid() || !m_codeEditor->isFoldable(block))
            return;
        m_codeEditor->toggleFold(block);
    }
    QWidget::mouseReleaseEvent(event);
}
