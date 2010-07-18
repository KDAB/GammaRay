

#include <KDE/KProcess>
#include <KDE/KStandardDirs>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>


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
  KProcess proc;
  proc.setEnv( "LD_PRELOAD", probe );
  proc.setProgram( args );
  return proc.execute();
}
