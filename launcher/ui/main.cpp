#include "launcherwindow.h"
#include "launchoptions.h"
#include "launcherfinder.h"

#include <QApplication>

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
      opts.execute(LauncherFinder::findLauncher(LauncherFinder::Injector));
    }
  }

  return result;
}