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
}

}

#endif // ENDOSCOPE_UTIL_H
