#include "util.h"

#include <QtCore/qobject.h>
#include <QtCore/QStringList>
#include <qsize.h>
#include <qpoint.h>
#include <qrect.h>
#include <qsizepolicy.h>
#include <qmetaobject.h>

using namespace Endoscope;

QString Util::displayString(const QObject* object)
{
  if ( !object )
    return "QObject(0x0)";
  if ( object->objectName().isEmpty() ) {
    return QString::fromLatin1( "%1 (%2)" )
      .arg( addressToString( object ) )
      .arg( object->metaObject()->className() );
  }
  return object->objectName();
}

static QString sizePolicyToString( QSizePolicy::Policy policy )
{
  const int index = QSizePolicy::staticMetaObject.indexOfEnumerator( "Policy" );
  const QMetaEnum metaEnum = QSizePolicy::staticMetaObject.enumerator( index );
  return QString::fromLatin1( metaEnum.valueToKey( policy ) );
}

QString Endoscope::Util::variantToString(const QVariant& value)
{
  switch ( value.type() ) {
    case QVariant::Point:
      return QString::fromLatin1( "%1x%2" ).arg( value.toPoint().x() ).arg( value.toPoint().y() );
    case QVariant::PointF:
      return QString::fromLatin1( "%1x%2" ).arg( value.toPointF().x() ).arg( value.toPointF().y() );
    case QVariant::Rect:
      return QString::fromLatin1( "%1x%2 %3x%4" )
        .arg( value.toRect().x() )
        .arg( value.toRect().y() )
        .arg( value.toRect().width() )
        .arg( value.toRect().height() );
    case QVariant::RectF:
      return QString::fromLatin1( "%1x%2 %3x%4" )
        .arg( value.toRectF().x() )
        .arg( value.toRectF().y() )
        .arg( value.toRectF().width() )
        .arg( value.toRectF().height() );
    case QVariant::Size:
      return QString::fromLatin1( "%1x%2" ).arg( value.toSize().width() ).arg( value.toSize().height() );
    case QVariant::SizeF:
      return QString::fromLatin1( "%1x%2" ).arg( value.toSizeF().width() ).arg( value.toSizeF().height() );
    case QVariant::SizePolicy:
      return QString::fromLatin1( "%1 x %2" )
        .arg( sizePolicyToString( value.value<QSizePolicy>().horizontalPolicy() ) )
        .arg( sizePolicyToString( value.value<QSizePolicy>().verticalPolicy() ) );
    case QVariant::StringList:
      return value.toStringList().join( ", " );
    default:
      return value.toString();
  }
}

QString Util::addressToString( const void* p )
{
    return (QLatin1String( "0x" ) + QString::number( reinterpret_cast<qlonglong>( p ), 16 ));
}
