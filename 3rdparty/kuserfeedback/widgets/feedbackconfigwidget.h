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

#ifndef KUSERFEEDBACK_FEEDBACKCONFIGWIDGET_H
#define KUSERFEEDBACK_FEEDBACKCONFIGWIDGET_H

#include "kuserfeedbackwidgets_export.h"

#include <provider.h>

#include <QWidget>

#include <memory>

namespace KUserFeedback {

class FeedbackConfigWidgetPrivate;
class Provider;

/*!
 * Configuration widget for telemetry and survey contributions.
 *
 * Use this rather than FeedbackConfigDialog if you want to embed the
 * feedback configuration for example into an existing configuration
 * dialog.
 * @see FeedbackConfigDialog
 */
class KUSERFEEDBACKWIDGETS_EXPORT FeedbackConfigWidget : public QWidget
{
    Q_OBJECT
public:
    /*! Create a new feedback provider configuration widget.
     *  @param parent The parent widget.
     */
    explicit FeedbackConfigWidget(QWidget *parent = nullptr);
    ~FeedbackConfigWidget();

    /*! Returns the feedback provider configured by this widget. */
    Provider* feedbackProvider() const;

    /*! Set the feedback provider that should be configured with this widget. */
    void setFeedbackProvider(Provider *provider);

    /*! Returns the telemetry level currently selected in the widget. */
    Provider::TelemetryMode telemetryMode() const;

    /*! Returns the survey interval currently selected in this widget. */
    int surveyInterval() const;

Q_SIGNALS:
    /*! Emitted when any changes are made to the configuration represented
     *  in this widget.
     */
    void configurationChanged();

protected:
    ///@cond internal
    bool eventFilter(QObject *receiver, QEvent *event) override;
    ///@endcond

private:
    Q_PRIVATE_SLOT(d, void telemetrySliderChanged())
    Q_PRIVATE_SLOT(d, void surveySliderChanged())
    std::unique_ptr<FeedbackConfigWidgetPrivate> d;
};

}

#endif // KUSERFEEDBACK_FEEDBACKCONFIGWIDGET_H
