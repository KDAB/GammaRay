/*
  textdocumentcontentview.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_TEXTDOCUMENTINSPECTOR_TEXTDOCUMENTCONTENTVIEW_H
#define GAMMARAY_TEXTDOCUMENTINSPECTOR_TEXTDOCUMENTCONTENTVIEW_H

#include <QTextEdit>

namespace GammaRay {
class TextDocumentContentView : public QTextEdit
{
    Q_OBJECT
public:
    explicit TextDocumentContentView(QWidget *parent = nullptr);

    void setShowBoundingBox(const QRectF &boundingBox);

protected:
    void paintEvent(QPaintEvent *e) override;

private:
    QRectF m_boundingBox;
};
}

#endif // GAMMARAY_TEXTDOCUMENTCONTENTVIEW_H
