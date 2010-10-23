#include "objectpropertymodel.h"

#include <KLocalizedString>
#include <QtCore/QMetaProperty>
#include <QtCore/QTimer>

using namespace Endoscope;

ObjectPropertyModel::ObjectPropertyModel(QObject* parent) :
  QAbstractTableModel(parent),
  m_updateTimer( new QTimer( this ) )
{
  connect( m_updateTimer, SIGNAL(timeout()), SLOT(doEmitChanged()) );
  m_updateTimer->setSingleShot( true );
}

void ObjectPropertyModel::setObject(QObject* object)
{
  if ( m_obj ) {
    disconnect( m_obj.data(), 0, this, SLOT(updateAll()) );
    disconnect( m_obj.data(), 0, this, SLOT(slotReset()) );
  }
  m_obj = object;
  if ( object ) {
    connect( object, SIGNAL(destroyed(QObject*)), SLOT(slotReset()) );
    for ( int i = 0; i < object->metaObject()->propertyCount(); ++i ) {
      const QMetaProperty prop = object->metaObject()->property( i );
      if ( prop.hasNotifySignal() )
        connect( object, QByteArray( "2" ) + prop.notifySignal().signature(), SLOT(updateAll()) );
    }
  }
  reset();
}

QVariant ObjectPropertyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if ( orientation == Qt::Horizontal && role == Qt::DisplayRole ) {
    switch ( section ) {
      case 0: return i18n( "Property" );
      case 1: return i18n( "Value" );
      case 2: return i18n( "Type" );
      case 3: return i18n( "Class" );
    }
  }
  return QAbstractItemModel::headerData(section, orientation, role);
}

void ObjectPropertyModel::updateAll()
{
  if ( m_updateTimer->isActive() )
    return;
  m_updateTimer->start( 100 );
}

void ObjectPropertyModel::doEmitChanged()
{
  emit dataChanged( index( 0, 0 ), index( rowCount() - 1, columnCount() - 1 ) );
}


#include "objectpropertymodel.h"
