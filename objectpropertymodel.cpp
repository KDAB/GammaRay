#include "objectpropertymodel.h"

#include <KDebug>
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
  disconnect( this, SLOT(updateAll()) );
  disconnect( this, SLOT(slotReset()) );
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

static QString translateBool( bool value )
{
  return value ? i18n( "yes" ) : i18n( "no" );
}

QVariant ObjectPropertyModel::data(const QModelIndex& index, int role) const
{
  if ( !index.isValid() || !m_obj || index.row() < 0 || index.row() >= m_obj.data()->metaObject()->propertyCount() )
    return QVariant();

  const QMetaProperty prop = m_obj.data()->metaObject()->property( index.row() );
  if ( role == Qt::DisplayRole || role == Qt::EditRole ) {
    if ( index.column() == 0 )
      return prop.name();
    else if ( index.column() == 1 )
      return prop.read( m_obj.data() );
    else if ( index.column() == 2 )
      return prop.typeName();
    else if ( index.column() == 3 ) {
      const QMetaObject* mo = m_obj.data()->metaObject();
      while ( mo->propertyOffset() > index.row() )
        mo = mo->superClass();
      return mo->className();
    }
  } else if ( role == Qt::ToolTipRole ) {
    const QString toolTip = i18n( "Constant: %1\nDesignable: %2\nFinal: %3\nResetable: %4\n"
      "Has notification: %5\nScriptable: %6\nStored: %7\nUser: %8\nWritable: %9",
      translateBool( prop.isConstant() ),
      translateBool( prop.isDesignable( m_obj.data() ) ),
      translateBool( prop.isFinal() ),
      translateBool( prop.isResettable() ),
      translateBool( prop.hasNotifySignal() ),
      translateBool( prop.isScriptable( m_obj.data() ) ),
      translateBool( prop.isStored( m_obj.data() ) ),
      translateBool( prop.isUser( m_obj.data() ) ),
      translateBool( prop.isWritable() ) );
    return toolTip;
  }

  return QVariant();
}

bool ObjectPropertyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if ( index.isValid() && m_obj && index.column() == 1 && index.row() >= 0
    && index.row() < m_obj.data()->metaObject()->propertyCount() && role == Qt::EditRole )
  {
    const QMetaProperty prop = m_obj.data()->metaObject()->property( index.row() );
    return prop.write( m_obj.data(), value );
  }
  return QAbstractItemModel::setData(index, value, role);
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

Qt::ItemFlags ObjectPropertyModel::flags(const QModelIndex& index) const
{
  const Qt::ItemFlags flags = QAbstractItemModel::flags(index);

  if ( !index.isValid() || !m_obj || index.column() != 1 || index.row() < 0
    || index.row() >= m_obj.data()->metaObject()->propertyCount() )
  {
    return flags;
  }

  const QMetaProperty prop = m_obj.data()->metaObject()->property( index.row() );
  if ( prop.isWritable() )
    return flags | Qt::ItemIsEditable;
  return flags;
}

int ObjectPropertyModel::columnCount(const QModelIndex& parent) const
{
  if ( parent.isValid() )
    return 0;
  return 4;
}

int ObjectPropertyModel::rowCount(const QModelIndex& parent) const
{
  if ( !m_obj || parent.isValid() )
    return 0;
  return m_obj.data()->metaObject()->propertyCount();
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
