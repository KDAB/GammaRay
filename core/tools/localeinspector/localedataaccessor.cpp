/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "localedataaccessor.h"

#include <QStringList>

using namespace GammaRay;

Q_GLOBAL_STATIC(LocaleDataAccessorRegistry, instance)

LocaleDataAccessorRegistry::LocaleDataAccessorRegistry()
{
}

LocaleDataAccessorRegistry *LocaleDataAccessorRegistry::instance()
{
  return ::instance();
}

QVector< LocaleDataAccessor * > LocaleDataAccessorRegistry::accessors()
{
  return ::instance()->m_accessors;
}

QVector< LocaleDataAccessor * > LocaleDataAccessorRegistry::enabledAccessors()
{
  return ::instance()->m_enabledAccessors;
}

void LocaleDataAccessorRegistry::registerAccessor(LocaleDataAccessor *accessor)
{
  ::instance()->m_accessors.push_back(accessor);
}

void LocaleDataAccessorRegistry::setAccessorEnabled(LocaleDataAccessor *accessor, bool enabled)
{
  QVector< LocaleDataAccessor * > &accessors = ::instance()->m_enabledAccessors;
  if (enabled && !accessors.contains(accessor)) {
    accessors.push_back(accessor);
  } else {
    int idx = accessors.indexOf(accessor);
    if (idx >= 0) {
      accessors.remove(idx);
    }
  }
  emit ::instance()->accessorsChanged();
}

LOCALE_SIMPLE_DEFAULT_ACCESSOR(Name,
  return locale.name();
)

LOCALE_SIMPLE_DEFAULT_ACCESSOR(Language,
  return QLocale::languageToString(locale.language());
)

LOCALE_SIMPLE_DEFAULT_ACCESSOR(Country,
  return QLocale::countryToString(locale.country());
)

#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
LOCALE_SIMPLE_ACCESSOR(Script,
  return QLocale::scriptToString(locale.script());
)

LOCALE_SIMPLE_ACCESSOR(Currency,
  return locale.currencySymbol(QLocale::CurrencySymbol) +
                       QLatin1String(" (") +
                       locale.currencySymbol(QLocale::CurrencyIsoCode) +
                       QLatin1String(") - ") +
                       locale.currencySymbol(QLocale::CurrencyDisplayName);
)

#endif
LOCALE_SIMPLE_ACCESSOR(TextDirection,
  return locale.textDirection() == Qt::LeftToRight ? "LTR" : "RTL";
)

LOCALE_SIMPLE_DEFAULT_ACCESSOR(TimeFormatLong,
  return locale.timeFormat(QLocale::LongFormat);
)

LOCALE_SIMPLE_ACCESSOR(TimeFormatShort,
  return locale.timeFormat(QLocale::ShortFormat);
)

LOCALE_SIMPLE_ACCESSOR(TimeFormatNarrow,
  return locale.timeFormat(QLocale::NarrowFormat);
)

LOCALE_SIMPLE_DEFAULT_ACCESSOR(DateFormatLong,
  return locale.dateFormat(QLocale::LongFormat);
)

LOCALE_SIMPLE_ACCESSOR(DateFormatShort,
  return locale.dateFormat(QLocale::ShortFormat);
)

LOCALE_SIMPLE_ACCESSOR(DateFormatNarrow,
  return locale.dateFormat(QLocale::NarrowFormat);
)

LOCALE_SIMPLE_DEFAULT_ACCESSOR(DateTimeFormatLong,
  return locale.dateTimeFormat(QLocale::LongFormat);
)

LOCALE_SIMPLE_ACCESSOR(DateTimeFormatShort,
  return locale.dateTimeFormat(QLocale::ShortFormat);
)

LOCALE_SIMPLE_ACCESSOR(DateTimeFormatNarrow,
  return locale.dateTimeFormat(QLocale::NarrowFormat);
)

LOCALE_SIMPLE_DEFAULT_ACCESSOR(MeasurementSystem,
  return locale.measurementSystem() == QLocale::ImperialSystem ? "Imperial" : "Metric";
)

LOCALE_SIMPLE_ACCESSOR(AmText,
  return locale.amText();
)

LOCALE_SIMPLE_ACCESSOR(PmText,
  return locale.pmText();
)

LOCALE_SIMPLE_DEFAULT_ACCESSOR(FloatFormat,
  return locale.toString(10000.1);
)

#if QT_VERSION >= QT_VERSION_CHECK(4, 8, 0)
LOCALE_SIMPLE_ACCESSOR(NativeCountry,
  return locale.nativeCountryName();
)

LOCALE_SIMPLE_ACCESSOR(NativeLanguage,
  return locale.nativeLanguageName();
)

LOCALE_SIMPLE_DEFAULT_ACCESSOR(FirstDayOfWeek,
  return QLocale().dayName(locale.firstDayOfWeek());
)

LOCALE_SIMPLE_DEFAULT_ACCESSOR(WeekDays,
  QStringList resultList;
  Q_FOREACH (const Qt::DayOfWeek &dayNumber, locale.weekdays()) {
    resultList << QLocale().dayName(dayNumber);
  }
  return QLocale().createSeparatedList(resultList);
)
#endif

#include "localedataaccessor.moc"
