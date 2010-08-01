#include "methodargumentmodel.h"
#include <KLocalizedString>
#include <KDebug>
#include <QtCore/qsharedpointer.h>

#define ARG(type, data) QSharedPointer<Endoscope::GenericSafeArgument>( new Endoscope::SafeArgument<type >( #type, data ) )

namespace Endoscope {

template <typename T>
class SafeArgument : public GenericSafeArgument
{
  public:
    SafeArgument( const char *aName, const T &aData ) :
      name( aName ),
      value( aData )
    {}

    operator QGenericArgument () const
    {
      return QGenericArgument( name.data(), static_cast<const void *>(&value) );
    }

  private:
    QByteArray name;
    T value;
};

}

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
  if ( index.row() >= 0 && index.row() < m_arguments.size() && role == Qt::EditRole ) {
    m_arguments[index.row()] = value;
    return true;
  }
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

QVector< QSharedPointer<GenericSafeArgument> > MethodArgumentModel::arguments() const
{
  QVector<QSharedPointer<GenericSafeArgument> > args( 10 );
  for ( int i = 0; i < rowCount(); ++i ) {
    const QVariant value = m_arguments.at( i );
    // TODO: handle remaining variant types, handle QVariant itself
    switch ( value.type() ) {
      case QVariant::Bool: args[i] = ARG( bool, value.toBool() ); break;
      case QVariant::Int: args[i] = ARG( int, value.toInt() ); break;
      case QVariant::UInt: args[i] = ARG( uint, value.toUInt() ); break;
      case QVariant::LongLong: args[i] = ARG( qlonglong, value.toLongLong() ); break;
      case QVariant::ULongLong: args[i] = ARG( qulonglong, value.toULongLong() ); break;
      case QVariant::Double: args[i] = ARG( double, value.toDouble() ); break;
      case QVariant::Char: args[i] = ARG( QChar, value.toChar() ); break;
      case QVariant::String: args[i] = ARG( QString, value.toString() ); break;
      case QVariant::StringList: args[i] = ARG( QStringList, value.toStringList() ); break;
      case QVariant::ByteArray: args[i] = ARG( QByteArray, value.toByteArray() ); break;
      default:
        kWarning() << "Unsupported argument type:" << value;
    }
  }
  for ( int i = rowCount(); i < 10; ++i ) {
    args[i] = QSharedPointer<GenericSafeArgument>( new GenericSafeArgument );
  }
  return args;
}

#undef ARG

#include "methodargumentmodel.moc"
