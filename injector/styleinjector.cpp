#include <config-endoscope.h>

#include "styleinjector.h"

#include <QProcess>

using namespace Endoscope;

int StyleInjector::launch(const QStringList& programAndArgs, const QString& probeDll, const QString& probeFunc)
{
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert( "ENDOSCOPE_STYLEINJECTOR_PROBEDLL", probeDll );
  env.insert( "ENDOSCOPE_STYLEINJECTOR_PROBEFUNC", probeFunc );

  QString qtPluginPath = env.value( "QT_PLUGIN_PATH" );
  if ( !qtPluginPath.isEmpty() )
    qtPluginPath.append( ":" );
  qtPluginPath.append( ENDOSCOPE_LIB_INSTALL_DIR "/qt4/plugins" );
  env.insert( "QT_PLUGIN_PATH", qtPluginPath );

  QProcess proc;
  proc.setProcessEnvironment( env );
  proc.setProcessChannelMode( QProcess::ForwardedChannels );

  QStringList args = programAndArgs;
  const QString program = args.takeFirst();
  args << QLatin1String("-style") << QLatin1String("endoscope-injector");
  proc.start( program, args );
  proc.waitForFinished( -1 );

  return proc.exitCode();
}

