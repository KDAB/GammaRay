
#include "config-endoscope.h"
#include "probefinder.h"
#include "injector/injectorfactory.h"

#include <QCoreApplication>
#include <QDebug>
#include <QStringList>

using namespace Endoscope;

int main( int argc, char** argv )
{
  QCoreApplication app( argc, argv );
  QStringList args = app.arguments();
  args.takeFirst(); // that's us

  QString injectorType;
  while ( !args.isEmpty() && args.first().startsWith('-') ) {
    const QString arg = args.takeFirst();
    if ( (arg == QLatin1String("-i") || arg == QLatin1String("--injector")) && !args.isEmpty() ) {
      injectorType = args.takeFirst();
    }
  }

  if ( args.isEmpty() ) {
    qWarning( "Nothing to probe. Usage: endoscope [--injector <injector>] <application> <args>" );
    return 1;
  }

  const QString probeDll = ProbeFinder::findProbe( QLatin1String("endoscope_probe") );
  AbstractInjector::Ptr injector;
  if ( injectorType.isEmpty() )
    injector = InjectorFactory::defaultInjectorForLaunch();
  else
    injector = InjectorFactory::createInjector( injectorType );

  if ( injector )
    return injector->launch( args, probeDll, QLatin1String("endoscope_probe_inject") );
  qWarning() << "Injector" << injectorType << "not found.";
  return 1;
}
