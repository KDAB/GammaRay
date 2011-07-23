#include "abstractinjector.h"

#include <QDebug>

using namespace Endoscope;

AbstractInjector::~AbstractInjector()
{
}

int AbstractInjector::launch(const QStringList& programAndArgs, const QString& probeDll, const QString& probeFunc)
{
  Q_UNUSED( programAndArgs );
  Q_UNUSED( probeDll );
  Q_UNUSED( probeFunc );
  qWarning() << "Injection on launch not supported by this injector.";
  return -1;
}

bool AbstractInjector::attach(int pid, const QString& probeDll, const QString& probeFunc)
{
  Q_UNUSED( pid );
  Q_UNUSED( probeDll );
  Q_UNUSED( probeFunc );
  qWarning() << "Attaching to a running process is not supported by this injector.";
  return false;
}
