/*
  util.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef GAMMARAY_UTIL_H
#define GAMMARAY_UTIL_H

#include <QtCore/QString>
#include <QtCore/QVariant>

class QObject;

namespace GammaRay {

namespace Util
{
  QString displayString(const QObject *object);
  QString variantToString(const QVariant &value);
  /// Returns a value representing @p value in a itemview decoration role
  QVariant decorationForVariant(const QVariant &value);
  QString addressToString(const void *p);
  QString addressToUid(const void *p);

  /**
   * Translates an enum or flag value into a human readable text.
   * @param value The numerical value. Type information from the QVariant
   *              are used to find the corresponding QMetaEnum.
   * @param typeName Use this if the @p value has type int
   *                 (e.g. the case for QMetaProperty::read).
   * @param object Additional QObject to search for QMetaEnums.
   */
  QString enumToString(const QVariant &value, const char *typeName = 0, QObject *object = 0);

  bool descendantOf(QObject *ascendant, QObject *obj);

  template <typename T>
  T *findParentOfType(QObject *object) {
    if (!object) {
      return 0;
    }
    if (qobject_cast<T*>(object)) {
      return qobject_cast<T*>(object);
    }
    return findParentOfType<T>(object->parent());
  }

  /// Returns an icon for the given object.
  QVariant iconForObject(QObject *obj);
}

}

#endif // GAMMARAY_UTIL_H
