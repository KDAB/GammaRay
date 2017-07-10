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

#ifndef KUSERFEEDBACK_ABSTRACTDATASOURCE_H
#define KUSERFEEDBACK_ABSTRACTDATASOURCE_H

#include "kuserfeedbackcore_export.h"
#include "provider.h"

#include <QCoreApplication>

QT_BEGIN_NAMESPACE
class QSettings;
QT_END_NAMESPACE

namespace KUserFeedback {

class AbstractDataSourcePrivate;

/*! Base class for data sources for telemetry data. */
class KUSERFEEDBACKCORE_EXPORT AbstractDataSource
{
public:
    virtual ~AbstractDataSource();

    /*! Returns the name of this data source.
     *  This is used as identifier towards the server and should
     *  not be shown to the user.
     *  @see description()
     *  @returns The data source identifier configured on the feedback server.
     */
    QString name() const;

    /*! Returns a human-readable, translated description of what
     *  this source provides.
     *  @see name()
     *  @returns A translated, human-readable string.
     */
    virtual QString description() const = 0;

    /*!
     * Returns the data gathered by this source.
     *
     * Implement this to return the data provided by this source.
     * One of the three following formats are expected:
     *  - scalar entries: QAssiciativeIterable
     *  - list entries: QSequentialIterable containing QAssociativeIterable
     *  - map entries: QAssiciativeIterable containing QAssociativeIterable
     *
     * The innermost QAssiciativeIterable must only contain one of the following
     * base types (which has to match the corresponding schema entry element):
     *  - QString
     *  - int
     *  - double
     *  - bool
     * All keys must be strings.
     *
     * @returns A variant complying with the above requirements.
     */
    virtual QVariant data() = 0;

    /*! Load persistent state for this data source.
     *  @param settings A QSettings object opened in a dedicated group for loading
     *  persistent data.
     */
    virtual void load(QSettings *settings);

    /*! Store persistent state for this data source.
     *  @param settings A QSettings object opened in a dedicated group for storing
     *  persistent data.
     */
    virtual void store(QSettings *settings);

    /*! Reset the persistent state of this data source.
     *  This is called after a successful submission of data, and can be used
     *  by sources that track differential rather than absolute data to reset
     *  their counters.
     *  @param settings A QSettings object opened in the dedicated group of this
     *  data source.
     */
    virtual void reset(QSettings *settings);

    /*! Returns which telemetry colleciton mode this data source belongs to.
     *  @return The telemetry collection category this source belongs to.
     */
    Provider::TelemetryMode telemetryMode() const;

    /*! Sets which telemetry colleciton mode this data source belongs to.
     * @param mode The data collection mode of this source.
     * Provider::NoTelemetry is not allowed here.
     */
    void setTelemetryMode(Provider::TelemetryMode mode);

protected:
    /*! Create a new data source named @p name.
     *  The name of the data source must match the corresponding
     *  product schema entry.
     *  @param name Must not be empty.
     *  @param mode The default telemetry mode.
     */
    explicit AbstractDataSource(const QString &name, Provider::TelemetryMode mode = Provider::DetailedUsageStatistics, AbstractDataSourcePrivate *dd = nullptr);

    /*! Set the name of this data source.
     *  The name should not change at runtime, this is only provided
     *  for enabling QML API for generic sources.
     *  @see name()
     */
    void setName(const QString &name);

    ///@cond internal
    class AbstractDataSourcePrivate* const d_ptr;
    ///@endcond

private:
    Q_DECLARE_PRIVATE(AbstractDataSource)
    Q_DISABLE_COPY(AbstractDataSource)
};
}

#endif // KUSERFEEDBACK_ABSTRACTDATASOURCE_H
