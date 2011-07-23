#ifndef ENDOSCOPE_PRELOADINJECTOR_H
#define ENDOSCOPE_PRELOADINJECTOR_H

#include "abstractinjector.h"
#include <qglobal.h>

#ifndef Q_OS_WIN

namespace Endoscope {

class PreloadInjector : public AbstractInjector
{
  public:
    virtual int launch(const QStringList& programAndArgs, const QString& probeDll, const QString& probeFunc);
};

}

#endif

#endif // ENDOSCOPE_PRELOADINJECTOR_H
