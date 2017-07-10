/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This program is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "abstractdatasource.h"
#include "abstractdatasource_p.h"

using namespace KUserFeedback;

AbstractDataSourcePrivate::AbstractDataSourcePrivate()
    : mode(Provider::DetailedUsageStatistics)
{
}

AbstractDataSourcePrivate::~AbstractDataSourcePrivate()
{
}

AbstractDataSource::AbstractDataSource(const QString &name, Provider::TelemetryMode mode, AbstractDataSourcePrivate* dd) :
    d_ptr(dd ? dd : new AbstractDataSourcePrivate)
{
    d_ptr->name = name;
    d_ptr->mode = mode;
}

AbstractDataSource::~AbstractDataSource()
{
    delete d_ptr;
}

QString AbstractDataSource::name() const
{
    return d_ptr->name;
}

void AbstractDataSource::setName(const QString& name)
{
    d_ptr->name = name;
}

void AbstractDataSource::load(QSettings *settings)
{
    Q_UNUSED(settings);
}

void AbstractDataSource::store(QSettings *settings)
{
    Q_UNUSED(settings);
}

void AbstractDataSource::reset(QSettings *settings)
{
    Q_UNUSED(settings);
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
