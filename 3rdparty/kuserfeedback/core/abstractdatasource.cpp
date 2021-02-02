/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include <QtCore/QSettings>

#include "abstractdatasource.h"
#include "abstractdatasource_p.h"

using namespace KUserFeedback;

static bool defaultActiveState() { return true; }
static QString activeStateKey() { return QStringLiteral("activeState"); }

static QString commonSettingsGroupName() { return QStringLiteral("dataSourceCommonSettings"); }

AbstractDataSourcePrivate::AbstractDataSourcePrivate()
    : mode(Provider::DetailedUsageStatistics)
    , active(defaultActiveState())
{
}

AbstractDataSourcePrivate::~AbstractDataSourcePrivate()
{
}

void AbstractDataSourcePrivate::storeCommonSettings(QSettings *settings)
{
    settings->beginGroup(commonSettingsGroupName());

    settings->setValue(activeStateKey(), active);

    settings->endGroup();
}

void AbstractDataSourcePrivate::loadCommonSettings(QSettings *settings)
{
    settings->beginGroup(commonSettingsGroupName());

    active = settings->value(activeStateKey(), defaultActiveState()).toBool();

    settings->endGroup();
}

// Please note that this function is supposed to be invoked in between
// data submissions, so be careful to don't reset flags that might be
// changed by a user via UI (e.g., active state)
void AbstractDataSourcePrivate::resetCommonSettings(QSettings *settings)
{
    Q_UNUSED(settings);
}

AbstractDataSource::AbstractDataSource(const QString &id, Provider::TelemetryMode mode)
    : AbstractDataSource(id, mode, nullptr)
{
}

AbstractDataSource::AbstractDataSource(const QString &id, Provider::TelemetryMode mode,
                                       AbstractDataSourcePrivate* dd)
    : d_ptr(dd ? dd : new AbstractDataSourcePrivate)
{
    d_ptr->id = id;
    d_ptr->mode = mode;
}

AbstractDataSource::~AbstractDataSource()
{
    delete d_ptr;
}

QString AbstractDataSource::id() const
{
    return d_ptr->id;
}

QString AbstractDataSource::name() const
{
    return {};
}

void AbstractDataSource::setId(const QString& id)
{
    d_ptr->id = id;
}

void AbstractDataSource::loadImpl(QSettings *settings)
{
    Q_UNUSED(settings);
}

void AbstractDataSource::storeImpl(QSettings *settings)
{
    Q_UNUSED(settings);
}

void AbstractDataSource::resetImpl(QSettings *settings)
{
    Q_UNUSED(settings);
}

void AbstractDataSource::load(QSettings *settings)
{
    Q_D(AbstractDataSource);
    d->loadCommonSettings(settings);

    loadImpl(settings);
}

void AbstractDataSource::store(QSettings *settings)
{
    Q_D(AbstractDataSource);
    d->storeCommonSettings(settings);

    storeImpl(settings);
}

void AbstractDataSource::reset(QSettings *settings)
{
    Q_D(AbstractDataSource);
    d->resetCommonSettings(settings);

    resetImpl(settings);
}

Provider::TelemetryMode AbstractDataSource::telemetryMode() const
{
    Q_D(const AbstractDataSource);

    Q_ASSERT(d->mode != Provider::NoTelemetry);
    if (d->mode == Provider::NoTelemetry)
        return Provider::DetailedUsageStatistics;
    return d->mode;
}

void AbstractDataSource::setTelemetryMode(Provider::TelemetryMode mode)
{
    Q_D(AbstractDataSource);
    Q_ASSERT(mode != Provider::NoTelemetry);
    d->mode = mode;
}

bool AbstractDataSource::isActive() const
{
    Q_D(const AbstractDataSource);
    return d->active;
}

void AbstractDataSource::setActive(bool active)
{
    Q_D(AbstractDataSource);
    d->active = active;
}
