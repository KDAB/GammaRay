#include "injectorfactory.h"

#include "detourinjector.h"
#include "gdbinjector.h"
#include "preloadinjector.h"
#include "styleinjector.h"
#include "windllinjector.h"

#include <QtCore/QString>

namespace Endoscope {

namespace InjectorFactory {

AbstractInjector::Ptr createInjector( const QString &name )
{
  if ( name == QLatin1String("gdb") )
    return AbstractInjector::Ptr( new GdbInjector );
  if ( name == QLatin1String("style") )
    return AbstractInjector::Ptr( new StyleInjector );
#ifndef Q_OS_WIN
  if ( name == QLatin1String("preload") )
    return AbstractInjector::Ptr( new PreloadInjector );
#else
  if ( name == QLatin1String("windll") )
    return AbstractInjector::Ptr( new WinDllInjector );
  if ( name == QLatin1String("detour") )
    return AbstractInjector::Ptr( new DetourInjector );
#endif
  return AbstractInjector::Ptr( 0 );
}

AbstractInjector::Ptr defaultInjectorForLaunch()
{
#ifndef Q_OS_WIN
  return createInjector( QLatin1String("preload") );
#else
  return createInjector( QLatin1String("windll") );
#endif
}

AbstractInjector::Ptr defaultInjectorForAttach()
{
  return createInjector( QLatin1String("gdb") );
}

}
}
