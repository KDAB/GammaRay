#ifndef ENDOSCOPE_GDBINJECTOR_H
#define ENDOSCOPE_GDBINJECTOR_H

#include <injector/abstractinjector.h>


namespace Endoscope {

class GdbInjector : public AbstractInjector
{
  public:
    virtual bool attach(int pid, const QString& probeDll, const QString& probeFunc);
};

}

#endif // ENDOSCOPE_GDBINJECTOR_H
