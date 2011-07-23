#ifndef ENDOSCOPE_INJECTORFACTORY_H
#define ENDOSCOPE_INJECTORFACTORY_H

#include "abstractinjector.h"

namespace Endoscope {

namespace InjectorFactory
{
   AbstractInjector::Ptr createInjector( const QString &name );

   AbstractInjector::Ptr defaultInjectorForLaunch();

   AbstractInjector::Ptr defaultInjectorForAttach();
}

}

#endif // ENDOSCOPE_INJECTORFACTORY_H
