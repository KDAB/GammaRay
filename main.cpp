

#include <KDE/KStandardDirs>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QProcess>

int main( int argc, char** argv )
{
  QCoreApplication app( argc, argv );
  const QString probe = KStandardDirs::locate( "lib", "libendoscope_probe.so" );
  qDebug() << probe;
  if ( probe.isEmpty() )
    qFatal( "Endoscope probe not found." );
  QStringList args = app.arguments();
  args.takeFirst(); // that's us
  if ( args.isEmpty() )
    qFatal( "Nothing to probe." );
  
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.insert( "LD_PRELOAD", probe );
  QProcess proc;
  proc.setProcessEnvironment( env );
  const QString program = args.takeFirst();
  proc.start( program, args );
  proc.waitForFinished( -1 );
  return proc.exitCode();
}
