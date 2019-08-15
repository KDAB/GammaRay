/*
    Copyright (C) 2017 Volker Krause <vkrause@kde.org>

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

#ifndef KUSERFEEDBACK_FEEDBACKCONFIGUICONTROLLER_H
#define KUSERFEEDBACK_FEEDBACKCONFIGUICONTROLLER_H

#include "kuserfeedbackcore_export.h"
#include "provider.h"

#include <QObject>

#include <memory>

namespace KUserFeedback {

class FeedbackConfigUiControllerPrivate;
class Provider;

/*! Logic/behavior of the feedback configuration UI.
 *  This is available for use in e.g. QtQuick-based UIs.
 *  @see FeedbackConfigWidget
 */
class KUSERFEEDBACKCORE_EXPORT FeedbackConfigUiController : public QObject
{
    Q_OBJECT
    /*! The Provider instance we are configuring. */
    Q_PROPERTY(KUserFeedback::Provider* feedbackProvider READ feedbackProvider WRITE setFeedbackProvider NOTIFY providerChanged)
    /*! Amount of telemetry modes supported by the provider. */
    Q_PROPERTY(int telemetryModeCount READ telemetryModeCount NOTIFY providerChanged)
    /*! Amount of supported survey modes. */
    Q_PROPERTY(int surveyModeCount READ surveyModeCount CONSTANT)
public:
    explicit FeedbackConfigUiController(QObject *parent = nullptr);
    ~FeedbackConfigUiController();

    /*! Returns the feedback provider to be configured. */
    Provider* feedbackProvider() const;
    /*! Set the feedback provider to configure. */
    void setFeedbackProvider(Provider *provider);

    /*! Amount of supported telemetry modes.
     *  This depends on what type of sources the provider actually has.
     */
    int telemetryModeCount() const;
    /*! Amount of supported survey modes. */
    int surveyModeCount() const;

    /*! Convert slider index to telemetry mode. */
    Q_INVOKABLE KUserFeedback::Provider::TelemetryMode telemetryIndexToMode(int index) const;
    /*! Convert telemetry mode to slider index. */
    Q_INVOKABLE int telemetryModeToIndex(KUserFeedback::Provider::TelemetryMode mode) const;

    /*! Telemetry mode short name. */
    Q_INVOKABLE QString telemetryModeName(int telemetryIndex) const;
    /*! Telemetry mode explanation text. */
    Q_INVOKABLE QString telemetryModeDescription(int telemetryIndex) const;
    /*! Detailed information about the data sources of the given telemetry mode index. */
    Q_INVOKABLE QString telemetryModeDetails(int telemetryIndex) const;

    /*! Convert slider index to survey interval. */
    Q_INVOKABLE int surveyIndexToInterval(int index) const;
    /*! Convert survey interval to slider index. */
    Q_INVOKABLE int surveyIntervalToIndex(int interval) const;

    /*! Survey mode explanation text. */
    Q_INVOKABLE QString surveyModeDescription(int surveyIndex) const;

Q_SIGNALS:
    /*! A provider-related setting has changed. */
    void providerChanged();

private:
    std::unique_ptr<FeedbackConfigUiControllerPrivate> d;
};

}

#endif // KUSERFEEDBACK_FEEDBACKCONFIGUICONTROLLER_H
