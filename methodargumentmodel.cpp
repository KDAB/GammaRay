#include "methodargumentmodel.h"
#include <KLocalizedString>

using namespace Endoscope;

MethodArgumentModel::MethodArgumentModel(QObject* parent): QAbstractTableModel(parent)
{
}

void MethodArgumentModel::setMethod(const QMetaMethod& method)
{
  m_method = method;
  m_arguments.clear();
  m_arguments.resize( method.parameterTypes().size() );
  for ( int i = 0; i < m_arguments.size(); ++i ) {
    const QByteArray typeName = method.parameterTypes().at( i );
    const QVariant::Type variantType = QVariant::nameToType( typeName );
    m_arguments[i] = QVariant( variantType );
  }
  reset();
}

QVariant MethodArgumentModel::data(const QModelIndex& index, int role) const
{
  if ( !m_method.signature() || m_arguments.isEmpty() || index.row() < 0 || index.row() >= m_arguments.size() )
    return QVariant();

  if ( role == Qt::DisplayRole || role == Qt::EditRole ) {
    const QVariant value = m_arguments.at( index.row() );
    const QByteArray parameterName = m_method.parameterNames().at( index.row() );
    const QByteArray parameterType = m_method.parameterTypes().at( index.row() );
    switch ( index.column() ) {
      case 0:
        if ( parameterName.isEmpty() )
          return i18n( "<unnamed> (%1)", QString::fromLatin1( parameterType ) );
        return parameterName;
      case 1:
        return value;
      case 2:
        return parameterType;
    }
  }
  return QVariant();
}

int MethodArgumentModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED( parent );
  return 3;
}

int MethodArgumentModel::rowCount(const QModelIndex& parent) const
{
  if ( parent.isValid() )
    return 0;
  return m_arguments.size();
}

bool MethodArgumentModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  return QAbstractItemModel::setData(index, value, role);
}

QVariant MethodArgumentModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if ( orientation == Qt::Horizontal && role == Qt::DisplayRole ) {
    switch ( section ) {
      case 0: return i18n( "Argument" );
      case 1: return i18n( "Value" );
      case 2: return i18n( "Type" );
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

Qt::ItemFlags MethodArgumentModel::flags(const QModelIndex& index) const
{
  const Qt::ItemFlags flags = QAbstractItemModel::flags(index);
  if ( index.column() == 1 )
    return flags | Qt::ItemIsEditable;
  return flags;
}

#include "methodargumentmodel.moc"
