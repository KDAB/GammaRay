#include "preloadinjector.h"

#ifndef Q_OS_WIN

#include <QProcess>

using namespace Endoscope;

int PreloadInjector::launch(const QStringList& programAndArgs, const QString& probeDll, const QString& probeFunc)
{
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
#ifdef Q_OS_MAC
  env.insert( "DYLD_INSERT_LIBRARIES", probeDll );
#else
  env.insert( "LD_PRELOAD", probeDll );
#endif
  QProcess proc;
  proc.setProcessEnvironment( env );
  proc.setProcessChannelMode( QProcess::ForwardedChannels );

  QStringList args = programAndArgs;
  const QString program = args.takeFirst();
  proc.start( program, args );
  proc.waitForFinished( -1 );

  return proc.exitCode();
}

#endif
