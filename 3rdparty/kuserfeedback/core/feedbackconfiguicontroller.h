/*
    Copyright (C) 2017 Volker Krause <vkrause@kde.org>

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

#ifndef USERFEEDBACK_FEEDBACKCONFIGUICONTROLLER_H
#define USERFEEDBACK_FEEDBACKCONFIGUICONTROLLER_H

#include "userfeedbackcore_export.h"
#include "provider.h"

#include <QObject>

#include <memory>

namespace UserFeedback {

class FeedbackConfigUiControllerPrivate;
class Provider;

/*! Logic/behavior of the feedback configuration UI.
 *  This is available for use in e.g. QtQuick-based UIs.
 *  @see FeedbackConfigWidget
 */
class USERFEEDBACKCORE_EXPORT FeedbackConfigUiController : public QObject
{
    Q_OBJECT
    /*! The Provider instance we are configuring. */
    Q_PROPERTY(UserFeedback::Provider* feedbackProvider READ feedbackProvider WRITE setFeedbackProvider NOTIFY providerChanged)
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
    Q_INVOKABLE UserFeedback::Provider::StatisticsCollectionMode telemetryIndexToMode(int index) const;
    /*! Convert telemetry mode to slider index. */
    Q_INVOKABLE int telemetryModeToIndex(UserFeedback::Provider::StatisticsCollectionMode mode) const;

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

#endif // USERFEEDBACK_FEEDBACKCONFIGUICONTROLLER_H
