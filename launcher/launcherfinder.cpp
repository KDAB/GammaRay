#include "launcherfinder.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

using namespace GammaRay;

const char* executableNames[] = { "gammaray", "gammaray-launcher", "gammaray-client" };

QString LauncherFinder::findLauncher(LauncherFinder::Type type)
{
  QString fileName = executableNames[type];
#ifdef Q_OS_WIN
  fileName += ".exe";
#endif
  const QFileInfo fi(QCoreApplication::applicationDirPath() + QDir::separator() + fileName);
  if (fi.isExecutable())
    return fi.absoluteFilePath();
  qWarning() << fileName << "not found in the expected location (" << QCoreApplication::applicationDirPath() << "), "
             << "continuing anyway, hoping for it to be in PATH.";
  qWarning() << "This is likely a setup problem.";
  return fileName;
}
