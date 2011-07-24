#ifndef ENDOSCOPE_STYLEINJECTOR_H
#define ENDOSCOPE_STYLEINJECTOR_H

#include <injector/abstractinjector.h>


namespace Endoscope {

class StyleInjector : public Endoscope::AbstractInjector
{

public:
    virtual int launch(const QStringList& programAndArgs, const QString& probeDll, const QString& probeFunc);
};

}

#endif // ENDOSCOPE_STYLEINJECTOR_H
