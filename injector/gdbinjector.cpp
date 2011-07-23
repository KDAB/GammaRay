#include "gdbinjector.h"

#include <QDebug>
#include <QProcess>
#include <QStringList>

#include <dlfcn.h>

using namespace Endoscope;

bool GdbInjector::attach(int pid, const QString& probeDll, const QString& probeFunc)
{
  Q_ASSERT( pid > 0 );
  QProcess proc;
  proc.setProcessChannelMode( QProcess::ForwardedChannels );
  proc.start( QLatin1String("gdb"), QStringList() << QLatin1String("-pid") << QString::number(pid) );
  if ( !proc.waitForStarted( -1 ) ) {
    qWarning() << proc.errorString();
    return -1;
  }

  proc.write( "sha dl\n" );
  proc.write( qPrintable( QString::fromLatin1("call dlopen(\"%1\", %2)\n").arg( probeDll ).arg( RTLD_NOW ) ) );
  proc.write( qPrintable( QString::fromLatin1("sha %1\n").arg( probeDll ) ) );
  proc.write( qPrintable( QString::fromLatin1("call %1()\n").arg( probeFunc ) ) );
  proc.write( "detach\n" );
  proc.write( "quit\n" );
  proc.waitForBytesWritten( -1 );

  proc.waitForFinished( -1 );
  return proc.exitCode();
}

