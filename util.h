#ifndef ENDOSCOPE_UTIL_H
#define ENDOSCOPE_UTIL_H

#include <QtCore/QString>
#include <QtCore/QVariant>

class QObject;

namespace Endoscope {

namespace Util
{
  QString displayString( const QObject *object );
  QString variantToString( const QVariant &value );
  QString addressToString( const void* p );

  template <typename T>
  T* findParentOfType( QObject *object )
  {
    if ( !object )
      return 0;
    if ( qobject_cast<T*>( object ) )
      return qobject_cast<T*>( object );
    return findParentOfType<T>( object->parent() );
  }
}

}

#endif // ENDOSCOPE_UTIL_H
