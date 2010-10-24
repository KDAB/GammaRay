#include "objectstaticpropertymodel.h"
#include "util.h"

#include <QtCore/QMetaProperty>

using namespace Endoscope;

ObjectStaticPropertyModel::ObjectStaticPropertyModel(QObject* parent):
  ObjectPropertyModel(parent)
{
}

static QString translateBool( bool value )
{
  return value ? QObject::tr( "yes" ) : QObject::tr( "no" );
}

QVariant ObjectStaticPropertyModel::data(const QModelIndex& index, int role) const
{
  if ( !index.isValid() || !m_obj || index.row() < 0 || index.row() >= m_obj.data()->metaObject()->propertyCount() )
    return QVariant();

  const QMetaProperty prop = m_obj.data()->metaObject()->property( index.row() );
  if ( role == Qt::DisplayRole ) {
    if ( index.column() == 0 )
      return prop.name();
    else if ( index.column() == 1 )
      return Util::variantToString( prop.read( m_obj.data() ) );
    else if ( index.column() == 2 )
      return prop.typeName();
    else if ( index.column() == 3 ) {
      const QMetaObject* mo = m_obj.data()->metaObject();
      while ( mo->propertyOffset() > index.row() )
        mo = mo->superClass();
      return mo->className();
    }
  } else if ( role == Qt::EditRole ) {
    if ( index.column() == 1 )
      return prop.read( m_obj.data() );
  } else if ( role == Qt::ToolTipRole ) {
    const QString toolTip = tr( "Constant: %1\nDesignable: %2\nFinal: %3\nResetable: %4\n"
      "Has notification: %5\nScriptable: %6\nStored: %7\nUser: %8\nWritable: %9" ).
      arg( translateBool( prop.isConstant() ) ).
      arg( translateBool( prop.isDesignable( m_obj.data() ) ) ).
      arg( translateBool( prop.isFinal() ) ).
      arg( translateBool( prop.isResettable() ) ).
      arg( translateBool( prop.hasNotifySignal() ) ).
      arg( translateBool( prop.isScriptable( m_obj.data() ) ) ).
      arg( translateBool( prop.isStored( m_obj.data() ) ) ).
      arg( translateBool( prop.isUser( m_obj.data() ) ) ).
      arg( translateBool( prop.isWritable() ) );
    return toolTip;
  }

  return QVariant();
}

bool ObjectStaticPropertyModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
  if ( index.isValid() && m_obj && index.column() == 1 && index.row() >= 0
    && index.row() < m_obj.data()->metaObject()->propertyCount() && role == Qt::EditRole )
  {
    const QMetaProperty prop = m_obj.data()->metaObject()->property( index.row() );
    return prop.write( m_obj.data(), value );
  }
  return ObjectPropertyModel::setData(index, value, role);
}

int ObjectStaticPropertyModel::columnCount(const QModelIndex& parent) const
{
  if ( parent.isValid() )
    return 0;
  return 4;
}

int ObjectStaticPropertyModel::rowCount(const QModelIndex& parent) const
{
  if ( !m_obj || parent.isValid() )
    return 0;
  return m_obj.data()->metaObject()->propertyCount();
}

Qt::ItemFlags ObjectStaticPropertyModel::flags(const QModelIndex& index) const
{
  const Qt::ItemFlags flags = ObjectPropertyModel::flags(index);

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


#include "objectstaticpropertymodel.moc"
