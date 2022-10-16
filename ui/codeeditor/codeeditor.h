/*
  codeeditor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CODEEDITOR_H
#define GAMMARAY_CODEEDITOR_H

#include "gammaray_ui_export.h"

#include <config-gammaray.h>

#include <QPlainTextEdit>

namespace KSyntaxHighlighting {
class Repository;
class SyntaxHighlighter;
}

namespace GammaRay {

class CodeEditorSidebar;

class GAMMARAY_UI_EXPORT CodeEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit CodeEditor(QWidget *parent = nullptr);
    ~CodeEditor() override;

    void setFileName(const QString &fileName);
    void setSyntaxDefinition(const QString &syntaxName);

protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void updateSidebarGeometry();
    void updateSidebarArea(const QRect &rect, int dy);
    void highlightCurrentLine();
    void syntaxSelected(QAction *action);

private:
    friend class CodeEditorSidebar;
    int sidebarWidth() const;
    int foldingBarWidth() const;
    void sidebarPaintEvent(QPaintEvent *event);
    void ensureHighlighterExists();

    QTextBlock blockAtPosition(int y) const;
    bool isFoldable(const QTextBlock &block) const;
    static bool isFolded(const QTextBlock &block);
    void toggleFold(const QTextBlock &startBlock);

    static KSyntaxHighlighting::Repository *s_repository;

    CodeEditorSidebar *m_sideBar;
    KSyntaxHighlighting::SyntaxHighlighter *m_highlighter;
};
}

#endif // GAMMARAY_CODEEDITOR_H
