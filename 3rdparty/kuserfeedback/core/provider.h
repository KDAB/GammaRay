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

#ifndef USERFEEDBACK_PROVIDER_H
#define USERFEEDBACK_PROVIDER_H

#include "userfeedbackcore_export.h"

#include <QMetaType>
#include <QObject>
#include <QUrl>

namespace UserFeedback {

class AbstractDataSource;
class ProviderPrivate;
class SurveyInfo;

/*! The central object managing data sources and transmitting feedback to the server.
 *
 *  The defaults for this class are very defensive, so in order to make it actually
 *  operational and submit data, there is a number of settings you need to set in
 *  code, namely submission intervals, encouragement settings and adding data sources.
 *  The settings about what data to submit (statisticsCollectionMode) and how often
 *  to bother the user with surveys (surveyInterval) should not be set to hardcoded
 *  values in code, but left as choices to the user.
 */
class USERFEEDBACKCORE_EXPORT Provider : public QObject
{
    Q_OBJECT
    /*! The interval in which the user accepts surveys.
     *  This should be configurable for the user.
     *  @c -1 indicates surveys are disabled.
     *  @see surveyInterval(), setSurveyInterval()
     */
    Q_PROPERTY(int surveyInterval READ surveyInterval WRITE setSurveyInterval NOTIFY surveyIntervalChanged)

    /*! The telemetry mode the user has configured.
     * This should be configurable for the user.
     * @see statisticsCollectionMode(), setStatisticsCollectionMode()
     */
    Q_PROPERTY(StatisticsCollectionMode statisticsCollectionMode READ statisticsCollectionMode WRITE setStatisticsCollectionMode NOTIFY statisticsCollectionModeChanged)

    /*! Unique product id as set on the feedback server.
     *  @see setProductIdentifier
     */
    Q_PROPERTY(QString productIdentifier READ productIdentifier WRITE setProductIdentifier NOTIFY providerSettingsChanged)

    /*! URL of the feedback server.
     *  @see setFeedbackServer
     */
    Q_PROPERTY(QUrl feedbackServer READ feedbackServer WRITE setFeedbackServer NOTIFY providerSettingsChanged)

    /*! Submission interval in days.
     *  @see setSubmissionInterval
     */
    Q_PROPERTY(int submissionInterval READ submissionInterval WRITE setSubmissionInterval NOTIFY providerSettingsChanged)

    /*! Application starts before an encouragement message is shown.
     *  @see setApplicationStartsUntilEncouragement
     */
    Q_PROPERTY(int applicationStartsUntilEncouragement
                READ applicationStartsUntilEncouragement
                WRITE setApplicationStartsUntilEncouragement
                NOTIFY providerSettingsChanged)

    /*! Application usage time in seconds before an encouragement message is shown.
     *  @see setApplicationUsageTimeUntilEncouragement
     */
    Q_PROPERTY(int applicationUsageTimeUntilEncouragement
                READ applicationUsageTimeUntilEncouragement
                WRITE setApplicationUsageTimeUntilEncouragement
                NOTIFY providerSettingsChanged)

    /*! Encouragement delay after application start in seconds.
     *  @see setEncouragementDelay
     */
    Q_PROPERTY(int encouragementDelay READ encouragementDelay WRITE setEncouragementDelay NOTIFY providerSettingsChanged)

    /*! Encouragement interval.
     *  @see setEncouragementInterval
     */
    Q_PROPERTY(int encouragementInterval READ encouragementInterval WRITE setEncouragementInterval NOTIFY providerSettingsChanged)

public:
    /*! Telemetry collection modes.
     *  Colleciton modes are inclusive, ie. higher modes always imply data from
     *  lower modes too.
     */
    enum StatisticsCollectionMode {
        NoStatistics, ///< Transmit no data at all.
        BasicSystemInformation = 0x10, ///< Transmit basic information about the system.
        BasicUsageStatistics = 0x20, ///< Transmit basic usage statistics.
        DetailedSystemInformation = 0x30, ///< Transmit detailed system information.
        DetailedUsageStatistics = 0x40, ///< Transmit detailed usage statistics.
    };
#ifndef QT4_MOC_WORKAROUND
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    Q_ENUM(StatisticsCollectionMode)
#else
    Q_ENUMS(StatisticsCollectionMode)
#endif
#else
    Q_ENUMS(StatisticsCollectionMode)
#endif

    /*! Create a new feedback provider.
     *  @param parent The parent object.
     */
    explicit Provider(QObject *parent = nullptr);
    ~Provider();

    /*! Returns the current product identifier. */
    QString productIdentifier() const;
    /*! Set the product identifier.
     *  This is used to distinguish independent products on the same server.
     *  If this is not specified, the product identifier is dervied from the application name
     *  organisation domain specified in QCoreApplication.
     *  @param productId Unique product identifier, as configured on the feedback server.
     */
    void setProductIdentifier(const QString &productId);

    /*! Returns the current feedback server URL. */
    QUrl feedbackServer() const;
    /*! Set the feedback server URL.
     *  This must be called with an appropriate URL for this class to be operational.
     *  @param url The URL of the feedback server.
     */
    void setFeedbackServer(const QUrl &url);

    /*! Returns the current submission interval.
     *  @return Days between telemetry submissions, or -1 if submission is off.
     */
    int submissionInterval() const;
    /*! Set the automatic submission interval in days.
     *  This must be called with a positive number for this class to be operational,
     *  as the default is -1 (no submission ever).
     */
    void setSubmissionInterval(int days);

    /*! Returns the current statistics collection mode.
     *  The default is NoStatistics.
     */
    StatisticsCollectionMode statisticsCollectionMode() const;

    /*! Set which statistics should be submitted. */
    void setStatisticsCollectionMode(StatisticsCollectionMode mode);

    /*! Adds a data source for statistical data collection.
     *  @param source The data source to add. The Provider takes ownership of @p source.
     *  @param mode The statistics collection mode this source belongs to. Data is only
     *  send to the server for this source is a sufficiently high collection mode is configured
     *  by the user. @c NoStatistics is not allowed.
     */
    void addDataSource(AbstractDataSource *source, StatisticsCollectionMode mode);

    /*! Returns all data sources that have been added to this provider.
     *  @see addDataSource
     */
    QVector<AbstractDataSource*> dataSources() const;

    /*! Returns the minimum time between two surveys in days.
     *  The default is -1 (no surveys enabled).
     */
    int surveyInterval() const;

    /*! Sets the minimum time in days between two surveys.
     *  @c -1 indicates no surveys should be requested.
     */
    void setSurveyInterval(int days);

    /*! Returns the amount of application starts before an encouragement message is shown. */
    int applicationStartsUntilEncouragement() const;
    /*! Set the amount of application starts until the encouragement message should be shown.
     *  The default is -1, ie. no encouragement based on application starts.
     *  @param starts The amount of application starts after which an encouragement
     *  message should be displayed.
     */
    void setApplicationStartsUntilEncouragement(int starts);

    /*! Returns the amount of application usage time before an encouragement message is shown. */
    int applicationUsageTimeUntilEncouragement() const;
    /*! Set the amount of usage time until the encouragement message should be shown.
     *  The default is -1, ie. no encouragement based on application usage time.
     *  @param secs Amount of seconds until the encouragement should be shown.
     */
    void setApplicationUsageTimeUntilEncouragement(int secs);

    /*! Returns the current encouragement delay in seconds. */
    int encouragementDelay() const;
    /*! Set the delay after application start for the earliest display of the encouragement message.
     *  The default is 300, ie. 5 minutes after the application start.
     *  @note This only adds an additional contraint on usage time and startup count based
     *  encouragement messages, it does not actually trigger encouragement messages itself.
     *
     *  @param secs Amount of seconds after the application start for the earliest display
     *  of an encouragement message.
     *
     *  @see setApplicationStartsUntilEncouragement, setApplicationUsageTimeUntilEncouragement
     */
    void setEncouragementDelay(int secs);

    /*! Returns the current encouragement interval. */
    int encouragementInterval() const;
    /*! Sets the interval after the encouragement should be repeated.
     *  Encouragement messages are only repeated if no feedback options have been enabled.
     *  The default is -1, that is no repeated encouragement at all.
     *  @param days Days between encouragement messages, 0 disables repeated encouragements.
     */
    void setEncouragementInterval(int days);

public Q_SLOTS:
    /*! Manually submit currently recorded data. */
    void submit();

    /*! Marks the given survey as completed. This avoids getting further notification
     *  about the same survey.
     */
    void surveyCompleted(const UserFeedback::SurveyInfo &info);

Q_SIGNALS:
    /*! Emitted whenever there is a new survey available that can be presented
     *  to the user.
     */
    void surveyAvailable(const UserFeedback::SurveyInfo &survey);

    /*! Indicate that the encouragement notice should be shown. */
    void showEncouragementMessage();

    /*! Emitted when the survey interval changed. */
    void surveyIntervalChanged();

    /*! Emitted when the statistics collection mode has changed. */
    void statisticsCollectionModeChanged();

    /*! Emitted when any provider setting changed. */
    void providerSettingsChanged();

private:
    friend class ProviderPrivate;
    ProviderPrivate * const d;
    Q_PRIVATE_SLOT(d, void aboutToQuit())
    Q_PRIVATE_SLOT(d, void submitFinished())
    Q_PRIVATE_SLOT(d, void emitShowEncouragementMessage())
    // for UI
    Q_PRIVATE_SLOT(d, QByteArray jsonData(UserFeedback::Provider::StatisticsCollectionMode))
    // for testing
    Q_PRIVATE_SLOT(d, void load())
    Q_PRIVATE_SLOT(d, void store())
};

}

Q_DECLARE_METATYPE(UserFeedback::Provider::StatisticsCollectionMode)

#endif // USERFEEDBACK_PROVIDER_H
