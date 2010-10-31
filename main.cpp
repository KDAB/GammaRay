
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QProcess>

int main( int argc, char** argv )
{
  QCoreApplication app( argc, argv );
  QStringList args = app.arguments();
  args.takeFirst(); // that's us
  if ( args.isEmpty() ) {
    qWarning( "Nothing to probe. Usage: endoscope <application> <args>" );
    return 1;
  }

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  if ( env.value( "LD_PRELOAD").isEmpty() ) {
    QStringList pldirs;
    pldirs << "/usr/local/lib64" << "/usr/local/lib"
           << "/opt/lib64" << "/opt/lib"
           << "/usr/lib64" << "/usr/lib";
    QDir::setSearchPaths( "preloads", pldirs );
    QFile plfile( "preloads:libendoscope_probe.so");
    if ( plfile.exists() ) {
      env.insert( "LD_PRELOAD", plfile.fileName() );
    } else {
      qWarning( "Cannot locate libendoscope_probe.so in the typical places.\n"
                "Try setting the $LD_PRELOAD environment variable to the fullpath,\n"
                "For example:\n"
                "  export LD_PRELOAD=/opt/lib64/libendoscope_probe.so\n"
                "Continuing nevertheless, some systems can also preload from just the library name...");
      env.insert("LD_PRELOAD", "libendoscope_probe.so" );
    }
  }
  QProcess proc;
  proc.setProcessEnvironment( env );
  proc.setProcessChannelMode( QProcess::ForwardedChannels );
  const QString program = args.takeFirst();
  proc.start( program, args );
  proc.waitForFinished( -1 );
  return proc.exitCode();
}
