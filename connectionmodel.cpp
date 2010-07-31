#include "connectionmodel.h"
#include "util.h"

#include <KLocalizedString>

using namespace Endoscope;

ConnectionModel::ConnectionModel(QObject* parent): QAbstractTableModel(parent)
{
}

void ConnectionModel::connectionAdded(QObject* sender, const char* signal, QObject* receiver, const char* method, Qt::ConnectionType type)
{
  if ( sender == this || receiver == this )
    return;
  beginInsertRows( QModelIndex(), m_connections.size(), m_connections.size() );
  Connection c;
  c.sender = sender;
  c.signal = QByteArray( signal );
  c.receiver = receiver;
  c.method = QByteArray( method );
  c.type = type;
  m_connections.push_back( c );
  endInsertRows();
}

void ConnectionModel::connectionRemoved(QObject* sender, const char* signal, QObject* receiver, const char* method)
{
  if ( sender == this || receiver == this )
    return;

  for ( int i = 0; i < m_connections.size(); ) {
    const Connection &con = m_connections.at( i );
    if ( (sender == 0 || con.sender.data() == sender)
      && (signal == 0 || con.signal == signal)
      && (receiver == 0 || con.receiver.data() == receiver)
      && (method == 0 || con.method == method) )
    {
      beginRemoveRows( QModelIndex(), i, i );
      m_connections.remove( i );
      endRemoveRows();
    } else { 
      ++i;
    }
  }
}

QVariant ConnectionModel::data(const QModelIndex& index, int role) const
{
  if ( !index.isValid() || index.row() < 0 || index.row() >= m_connections.size() )
    return QVariant();

  const Connection con = m_connections.at( index.row() );
  if ( role == Qt::DisplayRole ) {
    if ( index.column() == 0 ) {
      if ( con.sender )
        return Util::displayString( con.sender.data() );
      else
        return i18n( "<destroyed>" );
    }
    if ( index.column() == 1 )
      return con.signal;
    if ( index.column() == 2 ) {
      if ( con.receiver )
        return Util::displayString( con.receiver.data() );
      else
        return i18n( "<destroyed>" );
    }
    if ( index.column() == 3 )
      return con.method;
    if ( index.column() == 4 ) {
      switch ( con.type ) {
        case Qt::AutoCompatConnection: return QLatin1String( "AutoCompatConnection" );
        case Qt::AutoConnection: return QLatin1String( "AutoConnection" );
        case Qt::BlockingQueuedConnection: return QLatin1String( "BlockingQueuedConnection" );
        case Qt::DirectConnection: return QLatin1String( "DirectConnection" );
        case Qt::QueuedConnection: return QLatin1String( "QueuedConnection" );
        case Qt::UniqueConnection: return QLatin1String( "UniqueConnection" );
        default: return i18n( "Unknown connection type" );
      }
    }
  }
  return QVariant();
}

QVariant ConnectionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if ( orientation == Qt::Horizontal && role == Qt::DisplayRole ) {
    switch ( section ) {
      case 0: return i18n( "Sender" );
      case 1: return i18n( "Signal" );
      case 2: return i18n( "Receiver" );
      case 3: return i18n( "Method" );
      case 4: return i18n( "Connection Type" );
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

int ConnectionModel::columnCount(const QModelIndex& parent) const
{
  Q_UNUSED( parent );
  return 5;
}

int ConnectionModel::rowCount(const QModelIndex& parent) const
{
  if ( parent.isValid() )
    return 0;
  return m_connections.size();
}

#include "connectionmodel.moc"
