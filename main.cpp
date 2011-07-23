
#include "config-endoscope.h"
#include "probefinder.h"
#include "injector/injectorfactory.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>

#ifdef Q_OS_WIN
#include <windows.h>
#ifdef USE_DETOURS
#include <detours.h>
#else
#include "windllinjection.h"
#endif
#endif

using namespace Endoscope;

int main( int argc, char** argv )
{
  QCoreApplication app( argc, argv );
  QStringList args = app.arguments();
  args.takeFirst(); // that's us
  if ( args.isEmpty() ) {
    qWarning( "Nothing to probe. Usage: endoscope <application> <args>" );
    return 1;
  }

  const QString probeDll = ProbeFinder::findProbe( QLatin1String("endoscope_probe") );
  AbstractInjector::Ptr injector = InjectorFactory::defaultInjectorForLaunch();
  return injector->launch( args, probeDll, QLatin1String("endoscope_probe_inject") );
}
