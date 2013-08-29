#ifndef GAMMARAY_PROBESETTINGS_H
#define GAMMARAY_PROBESETTINGS_H

#include <QVariant>

namespace GammaRay {

/** General purpose settings of the probe provided by the launcher. */
namespace ProbeSettings
{
  QVariant value(const QString &key, const QVariant &defaultValue = QString());
}

}

#endif // GAMMARAY_PROBESETTINGS_H
