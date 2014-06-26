/*
  textdocumentinspector.cpp

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

#include "textdocumentinspector.h"

#include "textdocumentformatmodel.h"
#include "textdocumentmodel.h"

#include "objecttypefilterproxymodel.h"
#include "probeinterface.h"

#include "common/objectbroker.h"

#include <QItemSelection>
#include <QTextDocument>

using namespace GammaRay;

TextDocumentInspector::TextDocumentInspector(ProbeInterface *probe, QObject *parent):
  QObject(parent)
{
  ObjectTypeFilterProxyModel<QTextDocument> *documentFilter =
    new ObjectTypeFilterProxyModel<QTextDocument>(this);
  documentFilter->setSourceModel(probe->objectListModel());
  probe->registerModel("com.kdab.GammaRay.TextDocumentsModel", documentFilter);

  QItemSelectionModel *selectionModel = ObjectBroker::selectionModel(documentFilter);
  connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(documentSelected(QItemSelection,QItemSelection)));

  m_textDocumentModel = new TextDocumentModel(this);
  probe->registerModel("com.kdab.GammaRay.TextDocumentModel", m_textDocumentModel);

  selectionModel = ObjectBroker::selectionModel(m_textDocumentModel);
  connect(selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
          SLOT(documentElementSelected(QItemSelection,QItemSelection)));

  m_textDocumentFormatModel = new TextDocumentFormatModel(this);
  probe->registerModel("com.kdab.GammaRay.TextDocumentFormatModel", m_textDocumentFormatModel);
}

void TextDocumentInspector::documentSelected(const QItemSelection &selected,
                                             const QItemSelection &deselected)
{
  Q_UNUSED(deselected);
  const QModelIndex selectedRow = selected.first().topLeft();
  QObject *selectedObj = selectedRow.data(ObjectModel::ObjectRole).value<QObject*>();
  QTextDocument *doc = qobject_cast<QTextDocument*>(selectedObj);

  m_textDocumentModel->setDocument(doc);
}

void TextDocumentInspector::documentElementSelected(const QItemSelection &selected,
                                                    const QItemSelection &deselected)
{
  Q_UNUSED(deselected);
  if (selected.isEmpty()) {
    return;
  }
  const QModelIndex selectedRow = selected.first().topLeft();
  const QTextFormat f = selectedRow.data(TextDocumentModel::FormatRole).value<QTextFormat>();
  m_textDocumentFormatModel->setFormat(f);
}

