#include "util.h"

#include <QtCore/qobject.h>

using namespace Endoscope;

QString Util::displayString(const QObject* object)
{
  if ( object->objectName().isEmpty() ) {
    return QString::fromLatin1( "0x%1 (%2)" )
      .arg( QString::number( reinterpret_cast<qulonglong>( object ), 16 ) )
      .arg( object->metaObject()->className() );
  }
  return object->objectName();
}
