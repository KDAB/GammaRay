/*
  textdocumentinspector.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_TEXTDOCUMENTINSPECTOR_TEXTDOCUMENTINSPECTOR_H
#define GAMMARAY_TEXTDOCUMENTINSPECTOR_TEXTDOCUMENTINSPECTOR_H

#include <core/toolfactory.h>

#include <QPointer>
#include <QTextDocument>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QItemSelection;
class QItemSelectionModel;
QT_END_NAMESPACE

namespace GammaRay {
class TextDocumentModel;
class TextDocumentFormatModel;

class TextDocumentInspector : public QObject
{
    Q_OBJECT
public:
    explicit TextDocumentInspector(Probe *probe, QObject *parent = nullptr);

private slots:
    void documentSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void documentElementSelected(const QItemSelection &selected, const QItemSelection &deselected);
    void objectSelected(QObject *obj);

private:
    static void registerMetaTypes();

    QAbstractItemModel *m_documentsModel;
    QItemSelectionModel *m_documentSelectionModel;
    TextDocumentModel *m_textDocumentModel;
    TextDocumentFormatModel *m_textDocumentFormatModel;
};

class TextDocumentInspectorFactory : public QObject,
                                     public StandardToolFactory<QTextDocument, TextDocumentInspector>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_textdocumentinspector.json")
public:
    explicit TextDocumentInspectorFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_TEXTDOCUMENTINSPECTOR_H
