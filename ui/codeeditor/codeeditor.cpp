/*
  codeeditor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "codeeditor.h"
#include "codeeditorsidebar.h"

#ifdef HAVE_SYNTAX_HIGHLIGHTING
#include <SyntaxHighlighting/Definition>
#include <SyntaxHighlighting/SyntaxHighlighter>
#include <SyntaxHighlighting/Theme>
#endif

#include <QDebug>
#include <QPainter>
#include <QTextBlock>

using namespace GammaRay;

CodeEditor::CodeEditor(QWidget* parent) :
    QPlainTextEdit(parent),
    m_sideBar(new CodeEditorSidebar(this)),
    m_highlighter(Q_NULLPTR)
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
#endif

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateSidebarGeometry()));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateSidebarArea(QRect,int)));

    updateSidebarGeometry();
}

CodeEditor::~CodeEditor()
{
}

void CodeEditor::setFileName(const QString& fileName)
{
#ifdef HAVE_SYNTAX_HIGHLIGHTING
    const auto def = m_repository.definitionForFileName(fileName);
    if (!m_highlighter) {
        m_highlighter = new SyntaxHighlighting::SyntaxHighlighter(document());
        m_highlighter->setTheme((palette().color(QPalette::Base).lightness() < 128)
            ? m_repository.defaultTheme(SyntaxHighlighting::Repository::DarkTheme)
            : m_repository.defaultTheme(SyntaxHighlighting::Repository::LightTheme));
    }
    m_highlighter->setDefinition(def);
#endif
}

int CodeEditor::sidebarWidth() const
{
    int digits = 1;
    auto count = blockCount();
    while (count >= 10) {
        ++digits;
        count /= 10;
    }
    return 4 + fontMetrics().width(QLatin1Char('9')) * digits;
}

void CodeEditor::resizeEvent(QResizeEvent *event)
{
    QPlainTextEdit::resizeEvent(event);
    updateSidebarGeometry();
}

void CodeEditor::updateSidebarGeometry()
{
    setViewportMargins(sidebarWidth(), 0, 0, 0);
    const auto r = contentsRect();
    m_sideBar->setGeometry(QRect(r.left(), r.top(), sidebarWidth(), r.height()));
}

void CodeEditor::updateSidebarArea(const QRect& rect, int dy)
{
    if (dy)
        m_sideBar->scroll(0, dy);
    else
        m_sideBar->update(0, rect.y(), m_sideBar->width(), rect.height());
}

void CodeEditor::sidebarPaintEvent(QPaintEvent* event)
{
    QPainter painter(m_sideBar);
    painter.fillRect(event->rect(), palette().color(QPalette::Window));

    auto block = firstVisibleBlock();
    auto blockNumber = block.blockNumber();
    int top = blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + blockBoundingRect(block).height();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const auto number = QString::number(blockNumber + 1);
            painter.setPen(palette().color(QPalette::Text));
            painter.drawText(0, top, m_sideBar->width() - 2, fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + blockBoundingRect(block).height();
        ++blockNumber;
    }
}
