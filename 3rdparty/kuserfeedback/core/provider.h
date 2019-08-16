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

#ifndef KUSERFEEDBACK_PROVIDER_H
#define KUSERFEEDBACK_PROVIDER_H

#include "kuserfeedbackcore_export.h"

#include <QMetaType>
#include <QObject>
#include <QUrl>

namespace KUserFeedback {

class AbstractDataSource;
class ProviderPrivate;
class SurveyInfo;

/*! The central object managing data sources and transmitting feedback to the server.
 *
 *  The defaults for this class are very defensive, so in order to make it actually
 *  operational and submit data, there is a number of settings you need to set in
 *  code, namely submission intervals, encouragement settings and adding data sources.
 *  The settings about what data to submit (telemetryMode) and how often
 *  to bother the user with surveys (surveyInterval) should not be set to hardcoded
 *  values in code, but left as choices to the user.
 */
class KUSERFEEDBACKCORE_EXPORT Provider : public QObject
{
    Q_OBJECT
    /*! The global enabled state of the feedback functionality.
     *  If this is @c false, all feedback functionality has to be disabled completely.
     */
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

    /*! The interval in which the user accepts surveys.
     *  This should be configurable for the user.
     *  @c -1 indicates surveys are disabled.
     *  @see surveyInterval(), setSurveyInterval()
     */
    Q_PROPERTY(int surveyInterval READ surveyInterval WRITE setSurveyInterval NOTIFY surveyIntervalChanged)

    /*! The telemetry mode the user has configured.
     * This should be configurable for the user.
     * @see telemetryMode(), setTelemetryMode()
     */
    Q_PROPERTY(TelemetryMode telemetryMode READ telemetryMode WRITE setTelemetryMode NOTIFY telemetryModeChanged)

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

    /*! Times the application has to be started before an encouragement message is shown.
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
    enum TelemetryMode {
        NoTelemetry, ///< Transmit no data at all.
        BasicSystemInformation = 0x10, ///< Transmit basic information about the system.
        BasicUsageStatistics = 0x20, ///< Transmit basic usage statistics.
        DetailedSystemInformation = 0x30, ///< Transmit detailed system information.
        DetailedUsageStatistics = 0x40, ///< Transmit detailed usage statistics.
    };
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
    Q_ENUM(TelemetryMode)
#else
    Q_ENUMS(TelemetryMode)
#endif

    /*! Create a new feedback provider.
     *  @param parent The parent object.
     */
    explicit Provider(QObject *parent = nullptr);
    ~Provider();

    /*! Returns whether feedback functionality is enabled on this system.
     *  This should be checked everywhere showing feedback UI to the user
     *  to respect the global "kill switch" for this. Provider does check
     *  this internally for encouragements, surveys and telemetry submission.
     */
    bool isEnabled() const;
    /*! Set the global (user-wide) activation state for feedback functionality.
     *  @see isEnabled
     */
    void setEnabled(bool enabled);

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

    /*! Returns the current telemetry collection mode.
     *  The default is NoTelemetry.
     */
    TelemetryMode telemetryMode() const;

    /*! Set which telemetry data should be submitted. */
    void setTelemetryMode(TelemetryMode mode);

    /*! Adds a data source for telemetry data collection.
     *  @param source The data source to add. The Provider takes ownership of @p source.
     */
    void addDataSource(AbstractDataSource *source);

    /*! Returns all data sources that have been added to this provider.
     *  @see addDataSource
     */
    QVector<AbstractDataSource*> dataSources() const;

    /*! Returns a data source with matched @p id
     * @param id data source unique identifier
     * @return pointer to found data source or nullptr if data source is not found
     */
    AbstractDataSource *dataSource(const QString &id) const;

    /*! Returns the minimum time between two surveys in days.
     *  The default is -1 (no surveys enabled).
     */
    int surveyInterval() const;

    /*! Sets the minimum time in days between two surveys.
     *  @c -1 indicates no surveys should be requested.
     *  @c 0 indicates no minimum time between surveys at all (i.e. bother the user as often as you want).
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
    void surveyCompleted(const KUserFeedback::SurveyInfo &info);

    /*! Manually load settings of the provider and all added data sources.
     *  Automatically invoked after object construction and changing product ID.
     *  @note Potentially long operation.
     */
    void load();

    /*! Manually store settings of the provider and all added data sources.
     *  Will be autromatically invoked upon @p QCoreApplication::aboutToQuit signal.
     *  @note Potentially long operation.
     */
    void store();

Q_SIGNALS:
    /*! Emitted whenever there is a new survey available that can be presented
     *  to the user.
     */
    void surveyAvailable(const KUserFeedback::SurveyInfo &survey);

    /*! Indicate that the encouragement notice should be shown. */
    void showEncouragementMessage();

    /*! Emitted when the survey interval changed. */
    void surveyIntervalChanged();

    /*! Emitted when the telemetry collection mode has changed. */
    void telemetryModeChanged();

    /*! Emitted when any provider setting changed. */
    void providerSettingsChanged();

    /*! Emitted when the global enabled state changed. */
    void enabledChanged();

private:
    friend class ProviderPrivate;
    ProviderPrivate * const d;
    // for UI
    Q_PRIVATE_SLOT(d, QByteArray jsonData(KUserFeedback::Provider::TelemetryMode))
    // for testing
    Q_PRIVATE_SLOT(d, bool selectSurvey(const KUserFeedback::SurveyInfo&))
};

}

Q_DECLARE_METATYPE(KUserFeedback::Provider::TelemetryMode)

#endif // KUSERFEEDBACK_PROVIDER_H
