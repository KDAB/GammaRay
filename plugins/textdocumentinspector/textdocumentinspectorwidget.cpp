/*
  textdocumentinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "textdocumentinspectorwidget.h"
#include "ui_textdocumentinspectorwidget.h"
#include "textdocumentmodel.h"

#include <common/objectmodel.h>
#include <common/objectbroker.h>
#include <common/objectid.h>
#include <common/endpoint.h>

#include <ui/contextmenuextension.h>

#include <QMenu>
#include <QTextDocument>

using namespace GammaRay;

TextDocumentInspectorWidget::TextDocumentInspectorWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TextDocumentInspectorWidget)
    , m_stateManager(this)
{
    ui->setupUi(this);

    ui->documentList->header()->setObjectName("documentListHeader");
    ui->documentList->setDeferredResizeMode(0, QHeaderView::Stretch);
    ui->documentList->setDeferredResizeMode(1, QHeaderView::Stretch);
    ui->documentList->setModel(ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.TextDocumentsModel")));
    ui->documentList->setSelectionModel(ObjectBroker::selectionModel(ui->documentList->model()));
    connect(ui->documentList->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this, &TextDocumentInspectorWidget::documentSelected);
    connect(ui->documentList, &QWidget::customContextMenuRequested, this, &TextDocumentInspectorWidget::documentContextMenu);

    ui->documentTree->header()->setObjectName("documentTreeHeader");
    ui->documentTree->setDeferredResizeMode(0, QHeaderView::Stretch);
    ui->documentTree->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    ui->documentTree->setModel(ObjectBroker::model(QStringLiteral(
                                                       "com.kdab.GammaRay.TextDocumentModel")));
    ui->documentTree->setSelectionModel(ObjectBroker::selectionModel(ui->documentTree->model()));
    connect(ui->documentTree->selectionModel(),
            &QItemSelectionModel::selectionChanged,
            this, &TextDocumentInspectorWidget::documentElementSelected);

    ui->documentFormatView->header()->setObjectName("documentFormatViewHeader");
    ui->documentFormatView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->documentFormatView->setDeferredResizeMode(1, QHeaderView::Stretch);
    ui->documentFormatView->setDeferredResizeMode(2, QHeaderView::ResizeToContents);
    ui->documentFormatView->setModel(ObjectBroker::model(QStringLiteral(
                                                             "com.kdab.GammaRay.TextDocumentFormatModel")));

    if (Endpoint::instance()->isRemoteClient()) // FIXME: content preview doesn't work remotely yet
        ui->tabWidget->hide();

    m_stateManager.setDefaultSizes(ui->mainSplitter, UISizeVector() << 280 << -1 << -1);
    m_stateManager.setDefaultSizes(ui->structureSplitter, UISizeVector() << "50%" << "50%");
}

TextDocumentInspectorWidget::~TextDocumentInspectorWidget() = default;

void TextDocumentInspectorWidget::documentSelected(const QItemSelection &selected,
                                                   const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    if (selected.isEmpty()) {
        return;
    }
    const QModelIndex selectedRow = selected.first().topLeft();
    QObject *selectedObj = selectedRow.data(ObjectModel::ObjectRole).value<QObject *>();
    QTextDocument *doc = qobject_cast<QTextDocument *>(selectedObj);

    if (m_currentDocument) {
        disconnect(m_currentDocument.data(), &QTextDocument::contentsChanged,
                   this, &TextDocumentInspectorWidget::documentContentChanged);
    }
    m_currentDocument = QPointer<QTextDocument>(doc);

    if (doc) {
        ui->documentView->setDocument(doc);
        connect(doc, &QTextDocument::contentsChanged, this, &TextDocumentInspectorWidget::documentContentChanged);
        documentContentChanged();
    }
}

void TextDocumentInspectorWidget::documentElementSelected(const QItemSelection &selected,
                                                          const QItemSelection &deselected)
{
    Q_UNUSED(deselected);
    const QModelIndex selectedRow = selected.first().topLeft();

    const QRectF boundingBox = selectedRow.data(TextDocumentModel::BoundingBoxRole).toRectF();
    ui->documentView->setShowBoundingBox(boundingBox);
}

void TextDocumentInspectorWidget::documentContentChanged()
{
    ui->htmlView->setPlainText(m_currentDocument->toHtml());
}

void TextDocumentInspectorWidget::documentContextMenu(QPoint pos)
{
    const auto index = ui->documentList->indexAt(pos);
    if (!index.isValid())
        return;

    const auto objectId = index.data(ObjectModel::ObjectIdRole).value<ObjectId>();
    QMenu menu;
    ContextMenuExtension ext(objectId);
    ext.setLocation(ContextMenuExtension::Creation, index.data(ObjectModel::CreationLocationRole).value<SourceLocation>());
    ext.setLocation(ContextMenuExtension::Declaration, index.data(ObjectModel::DeclarationLocationRole).value<SourceLocation>());
    ext.populateMenu(&menu);

    menu.exec(ui->documentList->viewport()->mapToGlobal(pos));
}
