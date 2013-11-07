#ifndef GAMMARAY_LAUNCHERFINDER_H
#define GAMMARAY_LAUNCHERFINDER_H

#include <QString>

namespace GammaRay {

/** Utility function to find the various executables related to the launcher. */
namespace LauncherFinder
{
  enum Type {
    Injector,
    LauncherUI,
    Client
  };

  QString findLauncher(Type type);
}

}

#endif // GAMMARAY_LAUNCHERFINDER_H
