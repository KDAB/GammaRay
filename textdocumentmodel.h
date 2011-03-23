#ifndef ENDOSCOPE_TEXTDOCUMENTMODEL_H
#define ENDOSCOPE_TEXTDOCUMENTMODEL_H

#include <QStandardItemModel>
#include <qtextobject.h>

class QTextTable;
class QTextBlock;
class QTextFrame;
class QTextDocument;

namespace Endoscope {

class TextDocumentModel : public QStandardItemModel
{
  Q_OBJECT
  public:
    explicit TextDocumentModel( QObject * parent = 0 );

    void setDocument( QTextDocument *doc );

  private:
    void fillModel();
    void fillFrame( QTextFrame *frame, QStandardItem* parent );
    void fillFrameIterator( const QTextFrame::iterator &it, QStandardItem *parent );
    void fillTable( QTextTable *table, QStandardItem* parent );
    void fillBlock( const QTextBlock &block, QStandardItem *parent );
    QStandardItem *formatItem( const QTextFormat &format );

  private slots:
    void documentChanged();

  private:
    QTextDocument *m_document;
};

}

#endif
