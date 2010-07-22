#include "objectpropertymodel.h"
#include <QtCore/QMetaProperty>
#include <KLocalizedString>

using namespace Endoscope;

ObjectPropertyModel::ObjectPropertyModel(QObject* parent) :
  QAbstractTableModel(parent),
  m_obj( 0 )
{
}

void ObjectPropertyModel::setObject(QObject* object)
{
  m_obj = object;
  reset();
}

static QString translateBool( bool value )
{
  return value ? i18n( "yes" ) : i18n( "no" );
}

QVariant ObjectPropertyModel::data(const QModelIndex& index, int role) const
{
  if ( !index.isValid() || !m_obj || index.row() < 0 || index.row() >= m_obj->metaObject()->propertyCount() )
    return QVariant();

  const QMetaProperty prop = m_obj->metaObject()->property( index.row() );
  if ( role == Qt::DisplayRole || role == Qt::EditRole ) {
    if ( index.column() == 0 )
      return prop.name();
    else if ( index.column() == 1 )
      return prop.read( m_obj );
    else if ( index.column() == 2 )
      return prop.typeName();
    else if ( index.column() == 3 ) {
      const QMetaObject* mo = m_obj->metaObject();
      while ( mo->propertyOffset() > index.row() )
        mo = mo->superClass();
      return mo->className();
    }
  } else if ( role == Qt::ToolTipRole ) {
    const QString toolTip = i18n( "Constant: %1\nDesignable: %2\nFinal: %3\nResetable: %4\n"
      "Has notification: %5\nScriptable: %6\nStored: %7\nUser: %8\nWritable: %9",
      translateBool( prop.isConstant() ),
      translateBool( prop.isDesignable( m_obj ) ),
      translateBool( prop.isFinal() ),
      translateBool( prop.isResettable() ),
      translateBool( prop.hasNotifySignal() ),
      translateBool( prop.isScriptable( m_obj ) ),
      translateBool( prop.isStored( m_obj ) ),
      translateBool( prop.isUser( m_obj ) ),
      translateBool( prop.isWritable() ) );
    return toolTip;
  }

  return QVariant();
}

bool ObjectPropertyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if ( index.isValid() && m_obj && index.column() == 1 && index.row() >= 0
    && index.row() < m_obj->metaObject()->propertyCount() && role == Qt::EditRole )
  {
    const QMetaProperty prop = m_obj->metaObject()->property( index.row() );
    return prop.write( m_obj, value );
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
    || index.row() >= m_obj->metaObject()->propertyCount() )
  {
    return flags;
  }

  const QMetaProperty prop = m_obj->metaObject()->property( index.row() );
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
  return m_obj->metaObject()->propertyCount();
}

#include "objectpropertymodel.h"
