/*
  textdocumentmodel.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_TEXTDOCUMENTINSPECTOR_TEXTDOCUMENTMODEL_H
#define GAMMARAY_TEXTDOCUMENTINSPECTOR_TEXTDOCUMENTMODEL_H

#include "common/modelroles.h"

#include <QStandardItemModel>
#include <QTextObject>

QT_BEGIN_NAMESPACE
class QTextTable;
class QTextBlock;
class QTextFrame;
class QTextDocument;
QT_END_NAMESPACE

namespace GammaRay {
class TextDocumentModel : public QStandardItemModel
{
    Q_OBJECT
public:
    explicit TextDocumentModel(QObject *parent = nullptr);

    enum Roles
    {
        FormatRole = UserRole,
        BoundingBoxRole
    };

    void setDocument(QTextDocument *doc);

private:
    void fillModel();
    void fillFrame(QTextFrame *frame, QStandardItem *parent);
    void fillFrameIterator(const QTextFrame::iterator &it, QStandardItem *parent);
    void fillTable(QTextTable *table, QStandardItem *parent);
    void fillBlock(const QTextBlock &block, QStandardItem *parent);
    static QStandardItem *formatItem(const QTextFormat &format);
    void appendRow(QStandardItem *parent, QStandardItem *item, const QTextFormat &format,
                   const QRectF &boundingBox = QRectF());

private slots:
    void documentChanged();

private:
    QTextDocument *m_document;
};
}

#endif
