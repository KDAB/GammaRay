#ifndef ENDOSCOPE_UTIL_H
#define ENDOSCOPE_UTIL_H
#include <QtCore/QString>

class QObject;
namespace Endoscope {

namespace Util
{
  QString displayString( const QObject *object );
}

}

#endif // ENDOSCOPE_UTIL_H
