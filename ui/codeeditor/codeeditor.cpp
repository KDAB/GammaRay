/*
  codeeditor.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2016-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
#include <KSyntaxHighlighting/Definition>
#include <KSyntaxHighlighting/Repository>
#include <KSyntaxHighlighting/SyntaxHighlighter>
#include <KSyntaxHighlighting/Theme>
#endif

#include <QAction>
#include <QActionGroup>
#include <QCoreApplication>
#include <QDebug>
#include <QMenu>
#include <QPainter>
#include <QTextBlock>

using namespace GammaRay;

#ifdef HAVE_SYNTAX_HIGHLIGHTING
KSyntaxHighlighting::Repository *CodeEditor::s_repository = nullptr;
#endif

CodeEditor::CodeEditor(QWidget *parent) :
    QPlainTextEdit(parent),
    m_sideBar(new CodeEditorSidebar(this)),
    m_highlighter(nullptr)
{
    setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateSidebarGeometry()));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateSidebarArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateSidebarGeometry();
    highlightCurrentLine();
}

CodeEditor::~CodeEditor()
{
}

void CodeEditor::setFileName(const QString &fileName)
{
#ifdef HAVE_SYNTAX_HIGHLIGHTING
    ensureHighlighterExists();
    const auto def = s_repository->definitionForFileName(fileName);
    m_highlighter->setDefinition(def);
#else
    Q_UNUSED(fileName);
#endif
}

void CodeEditor::setSyntaxDefinition(const QString &syntaxName)
{
#ifdef HAVE_SYNTAX_HIGHLIGHTING
    ensureHighlighterExists();
    const auto def = s_repository->definitionForName(syntaxName);
    m_highlighter->setDefinition(def);
#else
    Q_UNUSED(syntaxName);
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
    return 4 + fontMetrics().width(QLatin1Char('9')) * digits + foldingBarWidth();
}

int CodeEditor::foldingBarWidth() const
{
#ifdef HAVE_SYNTAX_HIGHLIGHTING
    return fontMetrics().lineSpacing();
#else
    return 0;
#endif
}

void CodeEditor::contextMenuEvent(QContextMenuEvent *event)
{
    auto menu = createStandardContextMenu(event->pos());

#ifdef HAVE_SYNTAX_HIGHLIGHTING
    ensureHighlighterExists();
    menu->addSeparator();
    auto hlActionGroup = new QActionGroup(menu);
    hlActionGroup->setExclusive(true);
    auto hlGroupMenu = menu->addMenu(tr("Syntax Highlighting"));
    auto noHlAction = hlGroupMenu->addAction(QStringLiteral("None"));
    noHlAction->setCheckable(true);
    hlActionGroup->addAction(noHlAction);
    noHlAction->setChecked(!m_highlighter->definition().isValid());
    QMenu *hlSubMenu = nullptr;
    QString currentGroup;
    foreach (const auto &def, s_repository->definitions()) {
        if (def.isHidden())
            continue;

        if (currentGroup != def.section()) {
            currentGroup = def.section();
            hlSubMenu = hlGroupMenu->addMenu(def.translatedSection());
        }

        Q_ASSERT(hlSubMenu);
        if (hlSubMenu) {
            auto action = hlSubMenu->addAction(def.translatedName());
            action->setCheckable(true);
            action->setData(def.name());
            hlActionGroup->addAction(action);
            if (def.name() == m_highlighter->definition().name()) {
                action->setChecked(true);
            }
        }
    }
    connect(hlActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(syntaxSelected(QAction*)));
#endif

    menu->exec(event->globalPos());
    delete menu;
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

void CodeEditor::updateSidebarArea(const QRect &rect, int dy)
{
    if (dy) {
        m_sideBar->scroll(0, dy);
    } else {
        m_sideBar->update(0, rect.y(), m_sideBar->width(), rect.height());
    }
}

void CodeEditor::sidebarPaintEvent(QPaintEvent *event)
{
    QPainter painter(m_sideBar);
    painter.fillRect(event->rect(), palette().color(QPalette::Window));

    auto block = firstVisibleBlock();
    auto blockNumber = block.blockNumber();
    int top = blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + blockBoundingRect(block).height();
    const auto foldingMarkerSize = foldingBarWidth();

    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            const auto number = QString::number(blockNumber + 1);
            painter.setPen(palette().color(QPalette::Text));
            painter.drawText(0, top, m_sideBar->width() - 2 - foldingMarkerSize, fontMetrics().height(), Qt::AlignRight, number);
        }

        // folding marker
#ifdef HAVE_SYNTAX_HIGHLIGHTING
        if (block.isVisible() && isFoldable(block)) {
            QPolygonF polygon;
            if (isFolded(block)) {
                polygon << QPointF(foldingMarkerSize * 0.4, foldingMarkerSize * 0.25);
                polygon << QPointF(foldingMarkerSize * 0.4, foldingMarkerSize * 0.75);
                polygon << QPointF(foldingMarkerSize * 0.8, foldingMarkerSize * 0.5);
            } else {
                polygon << QPointF(foldingMarkerSize * 0.25, foldingMarkerSize * 0.4);
                polygon << QPointF(foldingMarkerSize * 0.75, foldingMarkerSize * 0.4);
                polygon << QPointF(foldingMarkerSize * 0.5, foldingMarkerSize * 0.8);
            }
            painter.save();
            painter.setRenderHint(QPainter::Antialiasing);
            painter.setPen(Qt::NoPen);
            painter.setBrush(palette().color(QPalette::Highlight));
            painter.translate(m_sideBar->width() - foldingMarkerSize, top);
            painter.drawPolygon(polygon);
            painter.restore();
        }
#endif

        block = block.next();
        top = bottom;
        bottom = top + blockBoundingRect(block).height();
        ++blockNumber;
    }
}

void CodeEditor::highlightCurrentLine()
{
    auto lineColor = palette().color(QPalette::Highlight);
    lineColor.setAlpha(32);

    QTextEdit::ExtraSelection selection;
    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor = textCursor();
    selection.cursor.clearSelection();

    QList<QTextEdit::ExtraSelection> extraSelections;
    extraSelections.append(selection);
    setExtraSelections(extraSelections);
}

void CodeEditor::syntaxSelected(QAction *action)
{
#ifdef HAVE_SYNTAX_HIGHLIGHTING
    Q_ASSERT(action);
    Q_ASSERT(s_repository);

    const auto defName = action->data().toString();
    const auto def = s_repository->definitionForName(defName);
    m_highlighter->setDefinition(def);
#else
    Q_UNUSED(action);
#endif
}

void CodeEditor::ensureHighlighterExists()
{
#ifdef HAVE_SYNTAX_HIGHLIGHTING
    if (!s_repository) {
        s_repository = new KSyntaxHighlighting::Repository;
        qAddPostRoutine([](){ delete s_repository; });
    }

    if (!m_highlighter) {
        m_highlighter = new KSyntaxHighlighting::SyntaxHighlighter(document());
        m_highlighter->setTheme(palette().color(QPalette::Base).lightness() < 128 ?
                                s_repository->defaultTheme(KSyntaxHighlighting::Repository::DarkTheme) :
                                s_repository->defaultTheme(KSyntaxHighlighting::Repository::LightTheme));
    }
#endif
}

QTextBlock CodeEditor::blockAtPosition(int y) const
{
    auto block = firstVisibleBlock();
    if (!block.isValid())
        return QTextBlock();

    int top = blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + blockBoundingRect(block).height();
    do {
        if (top <= y && y <= bottom) {
            return block;
        }
        block = block.next();
        top = bottom;
        bottom = top + blockBoundingRect(block).height();
    } while (block.isValid());
    return QTextBlock();
}

bool CodeEditor::isFoldable(const QTextBlock &block) const
{
#ifdef HAVE_SYNTAX_HIGHLIGHTING
    return m_highlighter->startsFoldingRegion(block);
#else
    Q_UNUSED(block);
    return false;
#endif
}

bool CodeEditor::isFolded(const QTextBlock &block) const
{
    if (!block.isValid())
        return false;

    const auto nextBlock = block.next();
    if (!nextBlock.isValid())
        return false;

    return !nextBlock.isVisible();
}

void CodeEditor::toggleFold(const QTextBlock &startBlock)
{
#ifdef HAVE_SYNTAX_HIGHLIGHTING
    // we also want to fold the last line of the region, therefore the ".next()"
    const auto endBlock = m_highlighter->findFoldingRegionEnd(startBlock).next();

    if (isFolded(startBlock)) {
        // unfold
        auto block = startBlock.next();
        while (block.isValid() && !block.isVisible()) {
            block.setVisible(true);
            block.setLineCount(block.layout()->lineCount());
            block = block.next();
        }

    } else {
        // fold
        auto block = startBlock.next();
        while (block.isValid() && block != endBlock) {
            block.setVisible(false);
            block.setLineCount(0);
            block = block.next();
        }
    }

    // redraw document
    document()->markContentsDirty(startBlock.position(), endBlock.position() - startBlock.position() + 1);
    // update scrollbars
    emit document()->documentLayout()->documentSizeChanged(document()->documentLayout()->documentSize());
#else
    Q_UNUSED(startBlock);
#endif
}
