/*
  util.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
/**
  @file
  This file is part of the GammaRay Plugin API and declares various utility
  methods needed when writing a plugin.

  @brief
  Declares various utility methods needed when writing a GammaRay plugin.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_UTIL_H
#define GAMMARAY_UTIL_H

#include "gammaray_core_export.h"

#include <QString>
#include <QVariant>

class QRect;
class QPainter;
class QObject;

namespace GammaRay {

/**
 * @brief GammaRay utilities.
 */
namespace Util {

  /**
   * Returns a human readable string name of the specified QObject.
   * This does include the type name.
   * @param object is a pointer to a valid or null QObject.
   *
   * @return a QString containing the human readable display string.
   */
  GAMMARAY_CORE_EXPORT QString displayString(const QObject *object);

  /**
   * Short display string for a QObject, either the object name or the address.
   * This does not include the type name.
   * @param object valid or 0 QObject
   */
  GAMMARAY_CORE_EXPORT QString shortDisplayString(const QObject *object);

  /**
   * Returns a string version (as a hex number starting with "0x") of the
   * memory address @p p.
   * @param p is a pointer to an address in memory.
   *
   * @return a QString containing the human readable address string.
   */
  GAMMARAY_CORE_EXPORT QString addressToString(const void *p);

  /**
   * Translates an enum or flag value into a human readable text.
   * @param value The numerical value. Type information from the QVariant
   *              are used to find the corresponding QMetaEnum.
   * @param typeName Use this if the @p value has type int
   *                 (e.g. the case for QMetaProperty::read).
   * @param object Additional QObject to search for QMetaEnums.
   *
   * @return a QString containing the string version of the specified @p value.
   */
  GAMMARAY_CORE_EXPORT QString enumToString(const QVariant &value,
                                       const char *typeName = 0,
                                       const QObject *object = 0);

  /**
   * Convenience wrapper for the above, in case the enum value is not available
   * in a QVariant already.
   */
  template <typename T>
  inline QString enumToString(T value, const char *typeName = 0, const QObject *object = 0)
  {
    return enumToString(QVariant::fromValue<T>(value), typeName, object);
  }

  /**
   * Returns a display string for @p method.
   * This includes return types and argument names, if available.
   */
  GAMMARAY_CORE_EXPORT QString prettyMethodSignature(const QMetaMethod &method);

  /**
   * Determines if the QObject @p obj is a descendant of the QObject @p ascendant.
   * @param ascendant is a pointer to a QObject.
   * @param object is a pointer to a QObject.
   *
   * @return true if @p obj is a descendant of @p ascendant; false otherwise.
   */
  GAMMARAY_CORE_EXPORT bool descendantOf(const QObject *ascendant, const QObject *object);

  /**
   * Finds the parent QObject of the specified type T, if such exists.
   * @param object is a pointer to a QObject.
   *
   * @return zero on failure; else a pointer to a data type T.
   *
   */
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

  /**
   * Returns an icon for the given object. In normal operation a QIcon is
   * returned containing the icon most closely associated with the data type
   * pointed to by @p object
   * @param object is a pointer to a QObject.
   *
   * @return on failure QVariant() is returned; else a QIcon
   */
  GAMMARAY_CORE_EXPORT QVariant iconForObject(const QObject *object);

  /**
   * Returns a suitable rich text tooltip string for @p object.
   * @param object a pointer to a valid or null QObject.
   */
  GAMMARAY_CORE_EXPORT QString tooltipForObject(const QObject *object);

  /**
   * Draws a transparency pattern, i.e. the common checkerboard pattern into @p rect.
   *
   * @p size The size of the individual checkerboard squares.
   */
  GAMMARAY_CORE_EXPORT void drawTransparencyPattern(QPainter *painter, const QRect &rect, int squareSize = 8);

  /**
   * Turns a signal index into a method index.
   * Signals indexes are used internally by QObject as an optimization and are
   * usually not exposed in public API. If you get them nevertheless, by using
   * internals of QObject this method turns them into method indexes that work
   * with public QMetaObject API.
   *
   * @param metaObject The meta object the signal belongs so
   * @since 2.2
   */
  GAMMARAY_CORE_EXPORT int signalIndexToMethodIndex(const QMetaObject* metaObject, int signalIndex);
}

}

#endif // GAMMARAY_UTIL_H
