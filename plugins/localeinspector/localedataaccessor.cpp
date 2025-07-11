/*
  localedataaccessor.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#include "localedataaccessor.h"

#include <QStringList>

using namespace GammaRay;

static QString dayNamesToString(const QLocale &locale, QString (QLocale::*accessor)(int, QLocale::FormatType) const,
                                QLocale::FormatType type)
{
    QStringList result;
    result.reserve(7);
    for (int i = 1; i <= 7; ++i)
        result.push_back((locale.*accessor)(i, type));
    return result.join(QStringLiteral(", "));
}

static QString monthNamesToString(const QLocale &locale, QString (QLocale::*accessor)(int, QLocale::FormatType) const,
                                  QLocale::FormatType type)
{
    QStringList result;
    result.reserve(12);
    for (int i = 1; i <= 12; ++i)
        result.push_back((locale.*accessor)(i, type));
    return result.join(QStringLiteral(", "));
}

LocaleDataAccessorRegistry::LocaleDataAccessorRegistry(QObject *parent)
    : QObject(parent)
{
    init();
}

LocaleDataAccessorRegistry::~LocaleDataAccessorRegistry()
{
    qDeleteAll(m_accessors);
}

QVector<LocaleDataAccessor *> LocaleDataAccessorRegistry::accessors()
{
    return m_accessors;
}

QVector<LocaleDataAccessor *> LocaleDataAccessorRegistry::enabledAccessors()
{
    return m_enabledAccessors;
}

void LocaleDataAccessorRegistry::registerAccessor(LocaleDataAccessor *accessor)
{
    m_accessors.push_back(accessor);
}

void LocaleDataAccessorRegistry::setAccessorEnabled(LocaleDataAccessor *accessor, bool enabled)
{
    QVector<LocaleDataAccessor *> &accessors = m_enabledAccessors;
    if (enabled && !accessors.contains(accessor)) {
        accessors.push_back(accessor);
        emit accessorAdded();
    } else {
        int idx = accessors.indexOf(accessor);
        if (idx >= 0) {
            accessors.remove(idx);
            emit accessorRemoved(idx);
        }
    }
}

void LocaleDataAccessorRegistry::init()
{
    LOCALE_SIMPLE_DEFAULT_ACCESSOR(Name,
                                   return locale.name();)

    LOCALE_SIMPLE_DEFAULT_ACCESSOR(Language,
                                   return QLocale::languageToString(locale.language());)


#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
    LOCALE_SIMPLE_DEFAULT_ACCESSOR(Country,
                                   return QLocale::countryToString(locale.country());)
#else
    LOCALE_SIMPLE_DEFAULT_ACCESSOR(Country,
                                   return QLocale::territoryToString(locale.territory());)
#endif

    LOCALE_SIMPLE_ACCESSOR(Script,
                           return QLocale::scriptToString(locale.script());)

    LOCALE_SIMPLE_ACCESSOR(Currency,
                           return locale.currencySymbol(QLocale::CurrencySymbol)
                           + QLatin1String(" (")
                           + locale.currencySymbol(QLocale::CurrencyIsoCode)
                           + QLatin1String(") - ")
                           + locale.currencySymbol(QLocale::CurrencyDisplayName);)

    LOCALE_SIMPLE_ACCESSOR(TextDirection,
                           return locale.textDirection() == Qt::LeftToRight ? QStringLiteral(
                                                                                  "LTR")
                                                                            : QStringLiteral("RTL");)

    LOCALE_SIMPLE_DEFAULT_ACCESSOR(TimeFormatLong,
                                   return locale.timeFormat(QLocale::LongFormat);)

    LOCALE_SIMPLE_ACCESSOR(TimeFormatShort,
                           return locale.timeFormat(QLocale::ShortFormat);)

    LOCALE_SIMPLE_ACCESSOR(TimeFormatNarrow,
                           return locale.timeFormat(QLocale::NarrowFormat);)

    LOCALE_SIMPLE_DEFAULT_ACCESSOR(DateFormatLong,
                                   return locale.dateFormat(QLocale::LongFormat);)

    LOCALE_SIMPLE_ACCESSOR(DateFormatShort,
                           return locale.dateFormat(QLocale::ShortFormat);)

    LOCALE_SIMPLE_ACCESSOR(DateFormatNarrow,
                           return locale.dateFormat(QLocale::NarrowFormat);)

    LOCALE_SIMPLE_DEFAULT_ACCESSOR(DateTimeFormatLong,
                                   return locale.dateTimeFormat(QLocale::LongFormat);)

    LOCALE_SIMPLE_ACCESSOR(DateTimeFormatShort,
                           return locale.dateTimeFormat(QLocale::ShortFormat);)

    LOCALE_SIMPLE_ACCESSOR(DateTimeFormatNarrow,
                           return locale.dateTimeFormat(QLocale::NarrowFormat);)

    LOCALE_SIMPLE_ACCESSOR(AmText,
                           return locale.amText();)

    LOCALE_SIMPLE_ACCESSOR(PmText,
                           return locale.pmText();)

    LOCALE_SIMPLE_ACCESSOR(DayNamesLong,
                           return dayNamesToString(locale, &QLocale::dayName, QLocale::LongFormat);)

    LOCALE_SIMPLE_ACCESSOR(DayNamesNarrow,
                           return dayNamesToString(locale, &QLocale::dayName,
                                                   QLocale::NarrowFormat);)

    LOCALE_SIMPLE_ACCESSOR(DayNamesShort,
                           return dayNamesToString(locale, &QLocale::dayName, QLocale::ShortFormat);)

    LOCALE_SIMPLE_ACCESSOR(StandaloneDayNamesLong,
                           return dayNamesToString(locale, &QLocale::standaloneDayName,
                                                   QLocale::LongFormat);)

    LOCALE_SIMPLE_ACCESSOR(StandaloneDayNamesNarrow,
                           return dayNamesToString(locale, &QLocale::standaloneDayName,
                                                   QLocale::NarrowFormat);)

    LOCALE_SIMPLE_ACCESSOR(StandaloneDayNamesShort,
                           return dayNamesToString(locale, &QLocale::standaloneDayName,
                                                   QLocale::ShortFormat);)

    LOCALE_SIMPLE_DEFAULT_ACCESSOR(FirstDayOfWeek,
                                   return QLocale().dayName(locale.firstDayOfWeek());)

    LOCALE_SIMPLE_DEFAULT_ACCESSOR(
        WeekDays,
        const auto wds = locale.weekdays();
        QStringList resultList;
        resultList.reserve(wds.size());
        for (Qt::DayOfWeek dayNumber : wds) {
            resultList << QLocale().dayName(dayNumber);
        } return QLocale()
            .createSeparatedList(resultList);)

    LOCALE_SIMPLE_ACCESSOR(MonthNamesLong,
                           return monthNamesToString(locale, &QLocale::monthName,
                                                     QLocale::LongFormat);)

    LOCALE_SIMPLE_ACCESSOR(MonthNamesNarrow,
                           return monthNamesToString(locale, &QLocale::monthName,
                                                     QLocale::NarrowFormat);)

    LOCALE_SIMPLE_ACCESSOR(MonthNamesShort,
                           return monthNamesToString(locale, &QLocale::monthName,
                                                     QLocale::ShortFormat);)

    LOCALE_SIMPLE_ACCESSOR(StandaloneMonthNamesLong,
                           return monthNamesToString(locale, &QLocale::standaloneMonthName,
                                                     QLocale::LongFormat);)

    LOCALE_SIMPLE_ACCESSOR(StandaloneMonthNamesNarrow,
                           return monthNamesToString(locale, &QLocale::standaloneMonthName,
                                                     QLocale::NarrowFormat);)

    LOCALE_SIMPLE_ACCESSOR(StandaloneMonthNamesLongShort,
                           return monthNamesToString(locale, &QLocale::standaloneMonthName,
                                                     QLocale::ShortFormat);)

    LOCALE_SIMPLE_ACCESSOR(BCP47,
                           return locale.bcp47Name();)

#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
    LOCALE_SIMPLE_ACCESSOR(NativeCountry,
                           return locale.nativeCountryName();)
#else
    LOCALE_SIMPLE_ACCESSOR(NativeCountry,
                           return locale.nativeTerritoryName();)
#endif
    LOCALE_SIMPLE_ACCESSOR(NativeLanguage,
                           return locale.nativeLanguageName();)

    LOCALE_SIMPLE_ACCESSOR(UiLanguages,
                           return locale.uiLanguages().join(QStringLiteral(", "));)

    LOCALE_SIMPLE_DEFAULT_ACCESSOR(MeasurementSystem,
                                   return locale.measurementSystem()
                                           == QLocale::ImperialSystem
                                       ? QStringLiteral(
                                             "Imperial")
                                       : QStringLiteral("Metric");)

    LOCALE_SIMPLE_DEFAULT_ACCESSOR(FloatFormat,
                                   return locale.toString(10000.1);)

    LOCALE_SIMPLE_ACCESSOR(DecimalPoint,
                           return locale.decimalPoint();)

    LOCALE_SIMPLE_ACCESSOR(GroupSeparator,
                           return locale.groupSeparator();)

    LOCALE_SIMPLE_ACCESSOR(Exponential,
                           return locale.exponential();)

    LOCALE_SIMPLE_ACCESSOR(Percent,
                           return locale.percent();)

    LOCALE_SIMPLE_ACCESSOR(PositiveSign,
                           return locale.positiveSign();)

    LOCALE_SIMPLE_ACCESSOR(NegativeSign,
                           return locale.negativeSign();)

    LOCALE_SIMPLE_ACCESSOR(ZeroDigit,
                           return locale.zeroDigit();)
}
