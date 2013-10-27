#include "launcherwindow.h"
#include "launchoptions.h"

#include <QApplication>
#include <QDir>

using namespace GammaRay;

int main(int argc, char** argv)
{
  QCoreApplication::setOrganizationName("KDAB");
  QCoreApplication::setOrganizationDomain("kdab.com");
  QCoreApplication::setApplicationName("GammaRay");

  QApplication app(argc, argv);
  LauncherWindow launcher;
  launcher.show();
  const int result = app.exec();

  if (launcher.result() == QDialog::Accepted) {
    const LaunchOptions opts = launcher.launchOptions();
    if (opts.isValid()) {
      QString launcherPath = QCoreApplication::applicationDirPath() + QDir::separator() + "gammaray";
#ifdef Q_OS_WIN
      launcherPath += ".exe";
#endif
      opts.execute(launcherPath);
    }
  }

  return result;
}