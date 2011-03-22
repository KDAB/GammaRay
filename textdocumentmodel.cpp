#include "textdocumentmodel.h"

#include <QTextCursor>
#include <QTextDocument>
#include <QTextFrame>
#include <boost/concept_check.hpp>

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
  appendRow( item );
  fillFrame( m_document->rootFrame(), item );
}

void TextDocumentModel::fillFrame(QTextFrame* frame, QStandardItem* parent)
{
  for ( QTextFrame::iterator it = frame->begin(); it != frame->end(); ++it ) {
    QStandardItem *item = new QStandardItem;
    if ( it.currentFrame() ) {
      item->setText( tr( "Frame" ) );
      parent->appendRow( item );
      fillFrame( it.currentFrame(), item );
    }
    const QTextBlock block = it.currentBlock();
    if ( block.isValid() ) {
      item->setText( tr( "Block: %1" ).arg( block.text() ) );
      parent->appendRow( item );
      fillBlock( block, item );
    }
  } 
}

void TextDocumentModel::fillBlock(const QTextBlock& block, QStandardItem* parent)
{
  for ( QTextBlock::iterator it = block.begin(); it != block.end(); ++it ) {
    QStandardItem *item = new QStandardItem( tr( "Fragment: %1" ).arg( it.fragment().text() ) );
    parent->appendRow( item );
  }
}

#include "textdocumentmodel.moc"
