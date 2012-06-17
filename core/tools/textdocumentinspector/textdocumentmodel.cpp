/*
  textdocumentmodel.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "textdocumentmodel.h"

#include <QAbstractTextDocumentLayout>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextFrame>
#include <QTextTable>

using namespace GammaRay;

TextDocumentModel::TextDocumentModel(QObject *parent)
  : QStandardItemModel(parent), m_document(0)
{
}

void TextDocumentModel::setDocument(QTextDocument *doc)
{
  if (m_document) {
    disconnect(m_document, SIGNAL(contentsChanged()), this, SLOT(documentChanged()));
  }

  m_document = doc;
  connect(m_document, SIGNAL(contentsChanged()), SLOT(documentChanged()));
  fillModel();
}

void TextDocumentModel::documentChanged()
{
  // TODO
  fillModel();
}

void TextDocumentModel::fillModel()
{
  clear();
  if (!m_document) {
    return;
  }

  QStandardItem *item = new QStandardItem(tr("Root Frame"));
  const QTextFormat f = m_document->rootFrame()->frameFormat();
  item->setData(QVariant::fromValue(f), FormatRole);
  QStandardItemModel::appendRow(QList<QStandardItem*>()
                                << item
                                << formatItem(m_document->rootFrame()->frameFormat()));
  fillFrame(m_document->rootFrame(), item);
  setHorizontalHeaderLabels(QStringList() << tr("Element") << tr("Format"));
}

void TextDocumentModel::fillFrame(QTextFrame *frame, QStandardItem *parent)
{
  for (QTextFrame::iterator it = frame->begin(); it != frame->end(); ++it) {
    fillFrameIterator(it, parent);
  }
}

void TextDocumentModel::fillFrameIterator(const QTextFrame::iterator &it, QStandardItem *parent)
{
  QStandardItem *item = new QStandardItem;
  if (QTextFrame *frame = it.currentFrame()) {
    const QRectF b = m_document->documentLayout()->frameBoundingRect(frame);
    QTextTable *table = qobject_cast<QTextTable*>(frame);
    if (table) {
      item->setText(tr("Table"));
      appendRow(parent, item, table->format(), b);
      fillTable(table, item);
    } else {
      item->setText(tr("Frame"));
      appendRow(parent, item, frame->frameFormat(), b);
      fillFrame(frame, item);
    }
  }
  const QTextBlock block = it.currentBlock();
  if (block.isValid()) {
    item->setText(tr("Block: %1").arg(block.text()));
    const QRectF b = m_document->documentLayout()->blockBoundingRect(block);
    appendRow(parent, item, block.blockFormat(), b);
    fillBlock(block, item);
  }
}

void TextDocumentModel::fillTable(QTextTable *table, QStandardItem *parent)
{
  for (int row = 0; row < table->rows(); ++row) {
    for (int col = 0; col < table->columns(); ++col) {
      QTextTableCell cell = table->cellAt(row, col);
      QStandardItem *item = new QStandardItem;
      item->setText(tr("Cell %1x%2").arg(row).arg(col));
      appendRow(parent, item, cell.format());
      for (QTextFrame::iterator it = cell.begin(); it != cell.end(); ++it) {
        fillFrameIterator(it, item);
      }
    }
  }
}

void TextDocumentModel::fillBlock(const QTextBlock &block, QStandardItem *parent)
{
  for (QTextBlock::iterator it = block.begin(); it != block.end(); ++it) {
    QStandardItem *item = new QStandardItem(tr("Fragment: %1").arg(it.fragment().text()));
    const QRectF b = m_document->documentLayout()->blockBoundingRect(block);
    appendRow(parent, item, it.fragment().charFormat(), b);
  }
}

QStandardItem *TextDocumentModel::formatItem(const QTextFormat &format)
{
  QStandardItem *item = new QStandardItem;
  if (!format.isValid()) {
    item->setText(tr("no format"));
  } else if (format.isImageFormat()) {
    const QTextImageFormat imgformat = format.toImageFormat();
    item->setText(tr("Image: %1").arg(imgformat.name()));
  } else {
    item->setText(tr("Format type: %1").arg(format.type()));
  }
  return item;
}

void TextDocumentModel::appendRow(QStandardItem *parent, QStandardItem *item,
                                  const QTextFormat &format, const QRectF &boundingBox)
{
  item->setData(QVariant::fromValue(format), FormatRole);
  item->setData(boundingBox, BoundingBoxRole);
  parent->appendRow(QList<QStandardItem*>() << item << formatItem(format));
}

#include "textdocumentmodel.moc"
