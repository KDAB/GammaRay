#ifndef GAMMARAY_STREAMOPERATORS_H
#define GAMMARAY_STREAMOPERATORS_H

namespace GammaRay {

/** Custom QDataStream streaming operators. */
namespace StreamOperators
{
  /** Call once early during startup. */
  void registerOperators();
}

}

#endif // GAMMARAY_STREAMOPERATORS_H
