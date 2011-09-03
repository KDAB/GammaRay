#include "textdocumentformatmodel.h"
#include <qmetaobject.h>
#include "util.h"

using namespace Endoscope;

static QMetaEnum propertyEnum()
{
  const int index = QTextFormat::staticMetaObject.indexOfEnumerator( "Property" );
  Q_ASSERT( index >= 0 );
  return QTextFormat::staticMetaObject.enumerator( index );
}

TextDocumentFormatModel::TextDocumentFormatModel(QObject* parent) : QAbstractTableModel(parent)
{
}

void TextDocumentFormatModel::setFormat(const QTextFormat& format)
{
  beginResetModel();
  m_format = format;
  endResetModel();
}

int TextDocumentFormatModel::rowCount(const QModelIndex& parent) const
{
  if ( !m_format.isValid() || parent.isValid() )
    return 0;
  return propertyEnum().keyCount();
}

int TextDocumentFormatModel::columnCount(const QModelIndex& parent) const
{
  return 3;
}

QVariant TextDocumentFormatModel::data(const QModelIndex& index, int role) const
{
  if ( role == Qt::DisplayRole && index.isValid() ) {
    const int enumValue = propertyEnum().value( index.row() );
    switch ( index.column() ) {
      case 0: return QString::fromLatin1( propertyEnum().key( index.row() ) );
      case 1: return Util::variantToString( m_format.property( enumValue ) );
      case 2: return QString::fromLatin1( m_format.property( enumValue ).typeName() );
    }
  }
  return QVariant();
}

QVariant TextDocumentFormatModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if ( orientation == Qt::Horizontal && role == Qt::DisplayRole ) {
    switch ( section ) {
      case 0: return tr("Property");
      case 1: return tr("Value");
      case 2: return tr("Type");
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

#include "textdocumentformatmodel.moc"
