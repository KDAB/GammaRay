#include "gdbinjector.h"

#include <QDebug>
#include <QProcess>
#include <QStringList>

#include <dlfcn.h>

using namespace Endoscope;

int GdbInjector::launch(const QStringList& programAndArgs, const QString& probeDll, const QString& probeFunc)
{
  QStringList gdbArgs;
  gdbArgs.push_back( QLatin1String("--args") );
  gdbArgs.append( programAndArgs );

  if ( !startGdb( gdbArgs ) )
    return -1;

  m_process->write( "break main\n" );
  m_process->write( "run\n" );
  m_process->write( "sha QtCore\n" );
  m_process->write( "break QCoreApplication::exec\n" );
  m_process->write( "continue\n" );

  return injectAndDetach( probeDll, probeFunc );
}

bool GdbInjector::attach(int pid, const QString& probeDll, const QString& probeFunc)
{
  Q_ASSERT( pid > 0 );
  if ( !startGdb( QStringList() << QLatin1String("-pid") << QString::number(pid) ) )
    return -1;
  return injectAndDetach( probeDll, probeFunc );
}

bool GdbInjector::startGdb(const QStringList& args)
{
  m_process.reset( new QProcess );
//   m_process->setProcessChannelMode( QProcess::ForwardedChannels );
  m_process->start( QLatin1String("gdb"), args );
  if ( !m_process->waitForStarted( -1 ) ) {
    qWarning() << m_process->errorString();
    return false;
  }
  return true;
}

int GdbInjector::injectAndDetach(const QString& probeDll, const QString& probeFunc)
{
  Q_ASSERT( m_process );
  m_process->write( "sha dl\n" );
  m_process->write( qPrintable( QString::fromLatin1("call dlopen(\"%1\", %2)\n").arg( probeDll ).arg( RTLD_NOW ) ) );
  m_process->write( qPrintable( QString::fromLatin1("sha %1\n").arg( probeDll ) ) );
  m_process->write( qPrintable( QString::fromLatin1("call %1()\n").arg( probeFunc ) ) );
  m_process->write( "detach\n" );
  m_process->write( "quit\n" );
  m_process->waitForBytesWritten( -1 );

  m_process->waitForFinished( -1 );
  return m_process->exitCode();
}
