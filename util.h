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
}

}

#endif // ENDOSCOPE_UTIL_H
