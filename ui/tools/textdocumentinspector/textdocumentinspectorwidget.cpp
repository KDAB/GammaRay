/*
  textdocumentinspectorwidget.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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
#include "core/tools/textdocumentinspector/textdocumentmodel.h"

#include <common/objectmodel.h>
#include <common/objectbroker.h>

#include <QTextDocument>

using namespace GammaRay;

TextDocumentInspectorWidget::TextDocumentInspectorWidget(QWidget *parent):
  QWidget(parent),
  ui(new Ui::TextDocumentInspectorWidget)
{
  ui->setupUi(this);

  ui->documentList->setModel(ObjectBroker::model("com.kdab.GammaRay.TextDocumentsModel"));
  QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(ui->documentList->model());
  ui->documentList->setSelectionModel(selectionModel);
  connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(documentSelected(QItemSelection,QItemSelection)));

  ui->documentTree->setModel(ObjectBroker::model("com.kdab.GammaRay.TextDocumentModel"));
  selectionModel = ObjectBroker::selectionModel(ui->documentTree->model());
  ui->documentTree->setSelectionModel(selectionModel);
  connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(documentElementSelected(QItemSelection,QItemSelection)));
  ui->documentFormatView->setModel(ObjectBroker::model("com.kdab.GammaRay.TextDocumentFormatModel"));
}

TextDocumentInspectorWidget::~TextDocumentInspectorWidget()
{
}

void TextDocumentInspectorWidget::documentSelected(const QItemSelection &selected,
                                             const QItemSelection &deselected)
{
  Q_UNUSED(deselected);
  const QModelIndex selectedRow = selected.first().topLeft();
  QObject *selectedObj = selectedRow.data(ObjectModel::ObjectRole).value<QObject*>();
  QTextDocument *doc = qobject_cast<QTextDocument*>(selectedObj);

  if (m_currentDocument) {
    disconnect(m_currentDocument, SIGNAL(contentsChanged()),
               this, SLOT(documentContentChanged()));
  }
  m_currentDocument = QPointer<QTextDocument>(doc);

  if (doc) {
    ui->documentView->setDocument(doc);
    connect(doc, SIGNAL(contentsChanged()), SLOT(documentContentChanged()));
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

