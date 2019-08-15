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
    std::unique_ptr<FeedbackConfigWidgetPrivate> d;
};

}

#endif // KUSERFEEDBACK_FEEDBACKCONFIGWIDGET_H
