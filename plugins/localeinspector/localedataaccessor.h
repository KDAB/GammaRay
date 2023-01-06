/*
  localedataaccessor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_LOCALEINSPECTOR_LOCALEDATAACCESSOR_H
#define GAMMARAY_LOCALEINSPECTOR_LOCALEDATAACCESSOR_H

#include <QLocale>
#include <QObject>
#include <QMetaType>
#include <QVariant>
#include <QVector>

namespace GammaRay {
struct LocaleDataAccessor;

class LocaleDataAccessorRegistry : public QObject
{
    Q_OBJECT
public:
    explicit LocaleDataAccessorRegistry(QObject *parent = nullptr);
    ~LocaleDataAccessorRegistry() override;

    void registerAccessor(LocaleDataAccessor *accessor);
    void setAccessorEnabled(LocaleDataAccessor *accessor, bool enabled);
    QVector<LocaleDataAccessor *> accessors();
    QVector<LocaleDataAccessor *> enabledAccessors();

Q_SIGNALS:
    void accessorAdded();
    void accessorRemoved(int idx);

private:
    void init();

private:
    QVector<LocaleDataAccessor *> m_accessors;
    QVector<LocaleDataAccessor *> m_enabledAccessors;
};

struct LocaleDataAccessor
{
    LocaleDataAccessor(LocaleDataAccessorRegistry *registry, bool defaultAccessor = false)
    {
        registry->registerAccessor(this);
        if (defaultAccessor)
            registry->setAccessorEnabled(this, true);
    }

    virtual ~LocaleDataAccessor() = default;

    virtual QString accessorName() = 0;

    QVariant data(const QLocale &locale, int role)
    {
        if (role == Qt::DisplayRole)
            return display(locale);
        return QVariant();
    }

    virtual QString display(const QLocale &)
    {
        return QString();
    }

    Q_DISABLE_COPY(LocaleDataAccessor)
};

#define LOCALE_DISPLAY_ACCESSOR(NAME)                                         \
    struct Locale##NAME##Accessor : LocaleDataAccessor                        \
    {                                                                         \
        explicit Locale##NAME##Accessor(LocaleDataAccessorRegistry *registry) \
            : LocaleDataAccessor(                                             \
                registry)                                                     \
        {                                                                     \
        }                                                                     \
        QString accessorName() override                                       \
        {                                                                     \
            return QStringLiteral(#NAME);                                     \
        }                                                                     \
        QString display(const QLocale &locale) override                       \
        {

#define LOCALE_DEFAULT_DISPLAY_ACCESSOR(NAME)                                 \
    struct Locale##NAME##Accessor : LocaleDataAccessor                        \
    {                                                                         \
        explicit Locale##NAME##Accessor(LocaleDataAccessorRegistry *registry) \
            : LocaleDataAccessor(                                             \
                registry, true)                                               \
        {                                                                     \
        }                                                                     \
                                                                              \
        QString accessorName() override                                       \
        {                                                                     \
            return QStringLiteral(#NAME);                                     \
        }                                                                     \
        QString display(const QLocale &locale) override                       \
        {

#define LOCALE_DISPLAY_ACCESSOR_END(NAME) \
    return QString();                     \
    }                                     \
    }                                     \
    ;                                     \
    new Locale##NAME##Accessor(this);

#define LOCALE_SIMPLE_ACCESSOR(NAME, IMPLEMENTATION) \
    LOCALE_DISPLAY_ACCESSOR(NAME)                    \
    IMPLEMENTATION                                   \
    LOCALE_DISPLAY_ACCESSOR_END(NAME)

#define LOCALE_SIMPLE_DEFAULT_ACCESSOR(NAME, IMPLEMENTATION) \
    LOCALE_DEFAULT_DISPLAY_ACCESSOR(NAME)                    \
    IMPLEMENTATION                                           \
    LOCALE_DISPLAY_ACCESSOR_END(NAME)
}

Q_DECLARE_METATYPE(GammaRay::LocaleDataAccessor *)

#endif
