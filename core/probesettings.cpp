#include "probesettings.h"

using namespace GammaRay;

QVariant ProbeSettings::value(const QString& key, const QVariant& defaultValue)
{
  // TODO: for now we simply use environment variables, which only works for launching, not attaching
  // this eventually needs to be extended to eg. shared memory or temporary files as communication channel

  const QByteArray v = qgetenv("GAMMARAY_" + key.toLocal8Bit());
  if (v.isEmpty())
    return defaultValue;

  switch (defaultValue.type()) {
    case QVariant::String:
      return QString::fromUtf8(v);
    case QVariant::Bool:
      return v == "true" || v == "1" || v == "TRUE";
    case QVariant::Int:
      return v.toInt();
    default:
      return v;
  }
}