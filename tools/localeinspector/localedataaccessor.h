/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation toolocale.

  Copyright (C) 2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#ifndef GAMMARAY_LOCALEDATAACCESSOR_H
#define GAMMARAY_LOCALEDATAACCESSOR_H

#include <QLocale>
#include <QVector>
#include <QObject>
#include <QMetaType>

namespace GammaRay {

struct LocaleDataAccessor;

class LocaleDataAccessorRegistry : public QObject
{
  Q_OBJECT
  public:
    LocaleDataAccessorRegistry();

    static LocaleDataAccessorRegistry *instance();

    static void registerAccessor(LocaleDataAccessor *accessor);
    static void setAccessorEnabled(LocaleDataAccessor *accessor, bool enabled);
    static QVector<LocaleDataAccessor*> accessors();
    static QVector<LocaleDataAccessor*> enabledAccessors();

  Q_SIGNALS:
    void accessorsChanged();

  private:
    QVector<LocaleDataAccessor*> m_accessors;
    QVector<LocaleDataAccessor*> m_enabledAccessors;
};

struct LocaleDataAccessor
{
  LocaleDataAccessor(bool defaultAccessor = false)
  {
    LocaleDataAccessorRegistry::registerAccessor(this);
    if (defaultAccessor) {
      LocaleDataAccessorRegistry::setAccessorEnabled(this, true);
    }
  }

  virtual QString accessorName() = 0;

  QString data(const QLocale &locale, int role)
  {
    if (role == Qt::DisplayRole) {
      return display(locale);
    }
    return QString();
  }
  virtual QString display(const QLocale &)
  {
    return QString();
  }
};

#define LOCALE_DISPLAY_ACCESSOR(NAME) \
struct Locale##NAME##Accessor : LocaleDataAccessor \
{ \
  QString accessorName() { return #NAME ; } \
  QString display(const QLocale &locale) \
  { \

#define LOCALE_DEFAULT_DISPLAY_ACCESSOR(NAME) \
struct Locale##NAME##Accessor : LocaleDataAccessor \
{ \
  Locale##NAME##Accessor() : LocaleDataAccessor(true) {} \
  \
  QString accessorName() { return #NAME ; } \
  QString display(const QLocale &locale) \
  { \

#define LOCALE_DISPLAY_ACCESSOR_END(NAME) \
    return QString(); \
  } \
} locale##NAME##Accessor;

#define LOCALE_SIMPLE_ACCESSOR(NAME, IMPLEMENTATION) \
  LOCALE_DISPLAY_ACCESSOR(NAME) \
  IMPLEMENTATION \
  LOCALE_DISPLAY_ACCESSOR_END(NAME)

#define LOCALE_SIMPLE_DEFAULT_ACCESSOR(NAME, IMPLEMENTATION) \
  LOCALE_DEFAULT_DISPLAY_ACCESSOR(NAME) \
  IMPLEMENTATION \
  LOCALE_DISPLAY_ACCESSOR_END(NAME)

}

Q_DECLARE_METATYPE(GammaRay::LocaleDataAccessor*)

#endif
