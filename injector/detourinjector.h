#ifndef ENDOSCOPE_DETOURINJECTOR_H
#define ENDOSCOPE_DETOURINJECTOR_H

#include <injector/abstractinjector.h>
#include <qglobal.h>

#ifdef Q_OS_WIN

namespace Endoscope {

class DetourInjector : public AbstractInjector
{
  public:
    virtual int launch(const QStringList& programAndArgs, const QString& probeDll, const QString& probeFunc);
};

}

#endif

#endif // ENDOSCOPE_DETOURINJECTOR_H
