#include "textdocumentmodel.h"

#include <QTextCursor>
#include <QTextDocument>
#include <QTextFrame>
#include <qtexttable.h>

using namespace Endoscope;

TextDocumentModel::TextDocumentModel(QObject* parent): QStandardItemModel(parent), m_document(0)
{
}

void TextDocumentModel::setDocument(QTextDocument* doc)
{
  m_document = doc;
  connect( m_document, SIGNAL(contentsChanged()), SLOT(documentChanged()) );
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
  if ( !m_document )
    return;

  QStandardItem* item = new QStandardItem( tr("Root Frame" ) );
  appendRow( QList<QStandardItem*>() << item << formatItem( m_document->rootFrame()->frameFormat() ) );
  fillFrame( m_document->rootFrame(), item );
  setHorizontalHeaderLabels( QStringList() << tr("Element") << tr("Format") );
}

void TextDocumentModel::fillFrame(QTextFrame* frame, QStandardItem* parent)
{
  for ( QTextFrame::iterator it = frame->begin(); it != frame->end(); ++it )
    fillFrameIterator( it, parent );
}

void TextDocumentModel::fillFrameIterator(const QTextFrame::iterator& it, QStandardItem* parent)
{
  QStandardItem *item = new QStandardItem;
  if ( QTextFrame *frame = it.currentFrame() ) {
    QTextTable *table = qobject_cast<QTextTable*>( frame );
    if ( table ) {
      item->setText( tr( "Table" ) );
      parent->appendRow( QList<QStandardItem*>() << item << formatItem( table->format() ) );
      fillTable( table, item );
    } else {
      item->setText( tr( "Frame" ) );
      parent->appendRow( QList<QStandardItem*>() << item << formatItem( frame->frameFormat() ) );
      fillFrame( frame, item );
    }
  }
  const QTextBlock block = it.currentBlock();
  if ( block.isValid() ) {
    item->setText( tr( "Block: %1" ).arg( block.text() ) );
    parent->appendRow( QList<QStandardItem*>() << item << formatItem( block.blockFormat() )  );
    fillBlock( block, item );
  }
}


void TextDocumentModel::fillTable(QTextTable* table, QStandardItem* parent)
{
  for ( int row = 0; row < table->rows(); ++row ) {
    for ( int col = 0; col < table->columns(); ++col ) {
      QTextTableCell cell = table->cellAt( row, col );
      QStandardItem *item = new QStandardItem;
      item->setText( tr( "Cell %1x%2" ).arg( row ).arg( col ) );
      parent->appendRow( QList<QStandardItem*>() << item << formatItem( cell.format() ) );
      for ( QTextFrame::iterator it = cell.begin(); it != cell.end(); ++it )
        fillFrameIterator( it, item );
    }
  }
}

void TextDocumentModel::fillBlock(const QTextBlock& block, QStandardItem* parent)
{
  for ( QTextBlock::iterator it = block.begin(); it != block.end(); ++it ) {
    QStandardItem *item = new QStandardItem( tr( "Fragment: %1" ).arg( it.fragment().text() ) );
    parent->appendRow( QList<QStandardItem*>() << item << formatItem( it.fragment().charFormat() ) );
  }
}

QStandardItem* TextDocumentModel::formatItem(const QTextFormat& format)
{
  QStandardItem *item = new QStandardItem;
  if ( !format.isValid() ) {
    item->setText( tr( "no format" ) );
  } else if ( format.isImageFormat() ) {
    const QTextImageFormat imgformat = format.toImageFormat();
    item->setText( tr("Image: %1").arg( imgformat.name() ) );
  } else {
    item->setText( tr("Format type: %1").arg( format.type() ) );
  }
  return item;
}

#include "textdocumentmodel.moc"
