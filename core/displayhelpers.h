#ifndef GAMMARAY_DISPLAYHELPERS_H
#define GAMMARAY_DISPLAYHELPERS_H

class QString;
class QVariant;
class QObject;

namespace GammaRay
{

namespace Util
{
  QString variantToString(const QVariant &value);
  /// Returns a value representing @p value in a itemview decoration role
  QVariant decorationForVariant(const QVariant &value);

  /**
   * Translates an enum or flag value into a human readable text.
   * @param value The numerical value. Type information from the QVariant
   *              are used to find the corresponding QMetaEnum.
   * @param typeName Use this if the @p value has type int
   *                 (e.g. the case for QMetaProperty::read).
   * @param object Additional QObject to search for QMetaEnums.
   */
  QString enumToString(const QVariant &value, const char *typeName = 0, QObject *object = 0);

  /// Returns an icon for the given object.
  QVariant iconForObject(QObject *obj);
};

}

#endif // GAMMARAY_DISPLAYHELPERS_H
