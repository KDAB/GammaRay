
#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QProcess>

int main( int argc, char** argv )
{
  QCoreApplication app( argc, argv );
  QStringList args = app.arguments();
  args.takeFirst(); // that's us
  if ( args.isEmpty() ) {
    qWarning( "Nothing to probe. Syntax: endoscope <application> <args>." );
    return 1;
  }

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert( "LD_PRELOAD", "libendoscope_probe.so" );
  QProcess proc;
  proc.setProcessEnvironment( env );
  proc.setProcessChannelMode( QProcess::ForwardedChannels );
  const QString program = args.takeFirst();
  proc.start( program, args );
  proc.waitForFinished( -1 );
  return proc.exitCode();
}
