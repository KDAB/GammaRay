#include "connectionfilterproxymodel.h"
#include "connectionmodel.h"

using namespace Endoscope;

ConnectionFilterProxyModel::ConnectionFilterProxyModel(QObject* parent) :
  QSortFilterProxyModel(parent),
  m_receiver( 0 ),
  m_sender( 0 )
{
  setDynamicSortFilter( true );
  sort( Qt::AscendingOrder );
}

void ConnectionFilterProxyModel::filterReceiver(QObject* receiver)
{
  m_receiver = receiver;
  invalidateFilter();
}

void ConnectionFilterProxyModel::filterSender(QObject* sender)
{
  m_sender = sender;
  invalidateFilter();
}

bool ConnectionFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
{
  const QModelIndex sourceIndex = sourceModel()->index( source_row, 0, source_parent );
  if ( m_sender && sourceIndex.data( ConnectionModel::SenderRole ).value<QObject*>() != m_sender )
    return false;
  if ( m_receiver && sourceIndex.data( ConnectionModel::ReceiverRole ).value<QObject*>() != m_receiver )
    return false;
  return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool ConnectionFilterProxyModel::filterAcceptsColumn(int source_column, const QModelIndex& source_parent) const
{
  if ( m_sender && source_column == 0 )
    return false;
  if ( m_receiver && source_column == 2 )
    return false;
  return QSortFilterProxyModel::filterAcceptsColumn(source_column, source_parent);
}

bool ConnectionFilterProxyModel::lessThan(const QModelIndex& left, const QModelIndex& right) const
{
  const bool leftValid = left.data( ConnectionModel::ConnectionValidRole ).toBool();
  const bool rightValid = right.data( ConnectionModel::ConnectionValidRole ).toBool();
  if ( leftValid == rightValid )
    return QSortFilterProxyModel::lessThan(left, right);
  else
    return rightValid;
}

#include "connectionfilterproxymodel.moc"
