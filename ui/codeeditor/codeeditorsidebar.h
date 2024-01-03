/*
  codeeditorsidebar.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_CODEEDITORSIDEBAR_H
#define GAMMARAY_CODEEDITORSIDEBAR_H

#include <QWidget>

namespace GammaRay {

class CodeEditor;

class CodeEditorSidebar : public QWidget
{
    Q_OBJECT
public:
    explicit CodeEditorSidebar(CodeEditor *editor);
    ~CodeEditorSidebar() override;

    QSize sizeHint() const override;

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    CodeEditor *m_codeEditor;
};
}

#endif // GAMMARAY_CODEEDITORSIDEBAR_H
