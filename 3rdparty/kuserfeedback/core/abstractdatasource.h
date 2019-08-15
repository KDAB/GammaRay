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

    /*! Returns the ID of this data source.
     *  This is used as identifier towards the server and should
     *  not be shown to the user.
     *  @see description()
     *  @returns The data source identifier configured on the feedback server.
     */
    QString id() const;

    /*! Returns a short name of this data source.
     *  Can be empty if short name is meaningless for this data source.
     *  @returns A translated, human-readable string.
     */
    virtual QString name() const;

    /*! Returns a human-readable, translated description of what
     *  this source provides.
     *  @see id()
     *  @returns A translated, human-readable string.
     */
    virtual QString description() const = 0;

    /*!
     * Returns the data gathered by this source.
     *
     * Implement this to return the data provided by this source.
     * One of the three following formats are expected:
     *  - scalar entries: QAssociativeIterable
     *  - list entries: QSequentialIterable containing QAssociativeIterable
     *  - map entries: QAssociativeIterable containing QAssociativeIterable
     *
     * The innermost QAssociativeIterable must only contain one of the following
     * base types (which has to match the corresponding schema entry element):
     *  - QString
     *  - int
     *  - double
     *  - bool
     *
     * All keys must be strings.
     *
     * @returns A variant complying with the above requirements.
     */
    virtual QVariant data() = 0;

    /*! Load persistent state for this data source.
     *  @param settings A QSettings object opened in a dedicated group for loading
     *  persistent data.
     */
    void load(QSettings *settings);

    /*! Store persistent state for this data source.
     *  @param settings A QSettings object opened in a dedicated group for storing
     *  persistent data.
     */
    void store(QSettings *settings);

    /*! Reset the persistent state of this data source.
     *  This is called after a successful submission of data, and can be used
     *  by sources that track differential rather than absolute data to reset
     *  their counters.
     *  @param settings A QSettings object opened in the dedicated group of this
     *  data source.
     */
    void reset(QSettings *settings);

    /*! Returns which telemetry colleciton mode this data source belongs to.
     *  @return The telemetry collection category this source belongs to.
     */
    Provider::TelemetryMode telemetryMode() const;

    /*! Sets which telemetry colleciton mode this data source belongs to.
     * @param mode The data collection mode of this source.
     * Provider::NoTelemetry is not allowed here.
     */
    void setTelemetryMode(Provider::TelemetryMode mode);

    /*! Checks whether this data source is active or not
     *  If the data source is not active, then collected
     *  data neither will be sent to a server nor appeared
     *  in the audit log.
     *  Data source is active by default.
     *  @return true if active, false otherwise
     */
    bool isActive() const;

    /*! Changes active state of the data source
     *  @param active The new active state for this data source
     */
    void setActive(bool active);

protected:
    /*! Create a new data source named @p name.
     *  The name of the data source must match the corresponding
     *  product schema entry.
     *  @param name Must not be empty.
     *  @param mode The default telemetry mode.
     */
    explicit AbstractDataSource(const QString &id,
                                Provider::TelemetryMode mode = Provider::DetailedUsageStatistics);

    ///@cond internal
    explicit AbstractDataSource(const QString &id,
                                Provider::TelemetryMode mode,
                                AbstractDataSourcePrivate *dd);
    ///@endcond

    /*! Set the ID of this data source.
     *  The ID should not change at runtime, this is only provided
     *  for enabling QML API for generic sources.
     *  @see id()
     */
    void setId(const QString &id);

    /*! Invoked by @p load() in order to load individual settings of this data source.
     *  @see load() description for further details.
     *  @param settings A QSettings object opened in a dedicated group for loading
     *  persistent data.
     */
    virtual void loadImpl(QSettings *settings);

    /*! Invoked by @p store() in order to store individual settings of this data source.
     *  @see store() description for further details.
     *  @param settings A QSettings object opened in a dedicated group for loading
     *  persistent data.
     */
    virtual void storeImpl(QSettings *settings);

    /*! Invoked by @p reset() in order to reset individual settings of this data source.
     *  @see reset() description for further details.
     *  @param settings A QSettings object opened in a dedicated group for loading
     *  persistent data.
     */
    virtual void resetImpl(QSettings *settings);

    ///@cond internal
    class AbstractDataSourcePrivate* const d_ptr;
    ///@endcond

private:
    Q_DECLARE_PRIVATE(AbstractDataSource)
    Q_DISABLE_COPY(AbstractDataSource)
};
}

#endif // KUSERFEEDBACK_ABSTRACTDATASOURCE_H
