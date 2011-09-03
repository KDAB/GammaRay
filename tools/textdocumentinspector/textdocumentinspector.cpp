#include "textdocumentinspector.h"
#include "ui_textdocumentinspector.h"

#include "textdocumentmodel.h"
#include "textdocumentformatmodel.h"

#include <objecttypefilterproxymodel.h>
#include <probeinterface.h>

#include <QTextDocument>

using namespace Endoscope;

TextDocumentInspector::TextDocumentInspector(ProbeInterface* probe, QWidget* parent):
  QWidget(parent),
  ui( new Ui::TextDocumentInspector )
{
  ui->setupUi( this );

  ObjectTypeFilterProxyModel<QTextDocument> *documentFilter = new ObjectTypeFilterProxyModel<QTextDocument>( this );
  documentFilter->setSourceModel( probe->objectListModel() );
  ui->documentList->setModel( documentFilter );
  connect( ui->documentList->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(documentSelected(QItemSelection,QItemSelection)) );
  m_textDocumentModel = new TextDocumentModel( this );
  ui->documentTree->setModel( m_textDocumentModel );
  connect( ui->documentTree->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(documentElementSelected(QItemSelection,QItemSelection)) );
  m_textDocumentFormatModel = new TextDocumentFormatModel( this );
  ui->documentFormatView->setModel( m_textDocumentFormatModel );
}

void TextDocumentInspector::documentSelected(const QItemSelection& selected, const QItemSelection& deselected)
{
  const QModelIndex selectedRow = selected.first().topLeft();
  QObject *selectedObj = selectedRow.data( ObjectListModel::ObjectRole ).value<QObject*>();
  QTextDocument *doc = qobject_cast<QTextDocument*>( selectedObj );
  if ( doc )
    ui->documentView->setDocument( doc );
  m_textDocumentModel->setDocument( doc );
}

void TextDocumentInspector::documentElementSelected(const QItemSelection& selected, const QItemSelection& deselected)
{
  const QModelIndex selectedRow = selected.first().topLeft();
  const QTextFormat f = selectedRow.data( TextDocumentModel::FormatRole ).value<QTextFormat>();
  m_textDocumentFormatModel->setFormat( f );
}

#include "textdocumentinspector.moc"
