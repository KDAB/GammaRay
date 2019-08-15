/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
