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

#include "feedbackconfigwidget.h"
#include "ui_feedbackconfigwidget.h"
#include "auditlogbrowserdialog.h"

#include <abstractdatasource.h>
#include <auditloguicontroller.h>
#include <feedbackconfiguicontroller.h>
#include <provider.h>

#include <QApplication>
#include <QDebug>
#include <QScrollBar>
#include <QStyle>

#include <algorithm>
#include <vector>

using namespace KUserFeedback;

namespace KUserFeedback {
class FeedbackConfigWidgetPrivate {
public:
    void telemetrySliderChanged();
    void surveySliderChanged();
    void applyPalette(QSlider *slider);

    FeedbackConfigUiController *controller;
    std::unique_ptr<Ui::FeedbackConfigWidget> ui;
    AuditLogUiController *auditLogController;
};

}

void FeedbackConfigWidgetPrivate::telemetrySliderChanged()
{
    if (ui->telemetrySlider->value() == 0) {
        ui->telemetryStack->setCurrentWidget(ui->noTelemetryPage);
    } else {
        ui->telemetryStack->setCurrentWidget(ui->telemetryPage);
        ui->telemetryLabel->setText(controller->telemetryModeDescription(ui->telemetrySlider->value()));
    }

    applyPalette(ui->telemetrySlider);

    if (!controller->feedbackProvider())
        return;

    if (!ui->rawTelemetryButton->isChecked()) {
        ui->telemetryDetails->setHtml(controller->telemetryModeDetails(ui->telemetrySlider->value()));
    } else {
        QByteArray jsonData;
        QMetaObject::invokeMethod(controller->feedbackProvider(), "jsonData", Q_RETURN_ARG(QByteArray, jsonData), Q_ARG(KUserFeedback::Provider::TelemetryMode, controller->telemetryIndexToMode(ui->telemetrySlider->value())));
        ui->telemetryDetails->setPlainText(QString::fromUtf8(jsonData.constData()));
    }
}

void FeedbackConfigWidgetPrivate::surveySliderChanged()
{
    if (!controller->feedbackProvider())
        return;

    ui->surveyLabel->setText(controller->surveyModeDescription(ui->surveySlider->value()));
    applyPalette(ui->surveySlider);
}

void FeedbackConfigWidgetPrivate::applyPalette(QSlider* slider)
{
    const auto ratio = (double)slider->value() / (double)slider->maximum();
    const auto red = qBound<double>(0.0, 2.0 - ratio * 2.0, 1.0);
    const auto green = qBound<double>(0.0, ratio * 2.0, 1.0);
    auto color = QColor(255 * red, 255 * green, 0);

    if (QApplication::palette().color(QPalette::Base).lightness() > 128)
        color = color.lighter(150);
    else
        color = color.darker(150);

    auto pal = slider->palette();
    pal.setColor(QPalette::Highlight, color);
    slider->setPalette(pal);
}


FeedbackConfigWidget::FeedbackConfigWidget(QWidget* parent)
    : QWidget(parent)
    , d(new FeedbackConfigWidgetPrivate)
{
    d->controller = new FeedbackConfigUiController(this);
    d->ui.reset(new Ui::FeedbackConfigWidget);
    d->ui->setupUi(this);
    d->ui->noTelemetryLabel->setText(d->controller->telemetryModeDescription(Provider::NoTelemetry));

    connect(d->ui->telemetrySlider, &QSlider::valueChanged, this, [this]() { d->telemetrySliderChanged(); });
    connect(d->ui->telemetrySlider, &QSlider::valueChanged, this, &FeedbackConfigWidget::configurationChanged);
    connect(d->ui->surveySlider, &QSlider::valueChanged, this, [this]() { d->surveySliderChanged(); });
    connect(d->ui->surveySlider, &QSlider::valueChanged, this, &FeedbackConfigWidget::configurationChanged);

    d->ui->rawTelemetryButton->setParent(d->ui->telemetryDetails);
    d->ui->rawTelemetryButton->setIcon(style()->standardPixmap(QStyle::SP_DialogHelpButton));
    d->ui->telemetryDetails->installEventFilter(this);
    connect(d->ui->rawTelemetryButton, &QAbstractButton::toggled, this, [this]() { d->telemetrySliderChanged(); });

    d->auditLogController = new AuditLogUiController(this);
    d->ui->auditLogLabel->setVisible(d->auditLogController->hasLogEntries());
    connect(d->auditLogController, &AuditLogUiController::logEntryCountChanged, this, [this]() {
        d->ui->auditLogLabel->setVisible(d->auditLogController->hasLogEntries());
    });
    connect(d->ui->auditLogLabel, &QLabel::linkActivated, this, [this](){
        AuditLogBrowserDialog dlg(this);
        dlg.setUiController(d->auditLogController);
        dlg.exec();
    });

    setEnabled(false); // see setFeedbackProvider
}

FeedbackConfigWidget::~FeedbackConfigWidget()
{
}

Provider* FeedbackConfigWidget::feedbackProvider() const
{
    return d->controller->feedbackProvider();
}

void FeedbackConfigWidget::setFeedbackProvider(Provider* provider)
{
    d->controller->setFeedbackProvider(provider);
    if (!provider) {
        setEnabled(false);
        return;
    }

    const auto hasTelemetry = d->controller->telemetryModeCount() > 1;
    d->ui->telemetrySlider->setEnabled(hasTelemetry);
    d->ui->telemetryStack->setEnabled(hasTelemetry);
    if (hasTelemetry)
        d->ui->telemetrySlider->setMaximum(d->controller->telemetryModeCount() - 1);

    d->ui->telemetrySlider->setValue(d->controller->telemetryModeToIndex(provider->telemetryMode()));
    d->ui->surveySlider->setValue(d->controller->surveyIntervalToIndex(provider->surveyInterval()));
    d->surveySliderChanged(); // update the description even if nothing changed initially

    setEnabled(provider);
}

bool FeedbackConfigWidget::eventFilter(QObject* receiver, QEvent* event)
{
    if (receiver == d->ui->telemetryDetails) {
        d->ui->rawTelemetryButton->move(
            d->ui->telemetryDetails->width()
                - d->ui->rawTelemetryButton->width()
                - style()->pixelMetric(QStyle::PM_LayoutRightMargin)
                - (d->ui->telemetryDetails->verticalScrollBar()->isVisible() ? d->ui->telemetryDetails->verticalScrollBar()->width() : 0),
            d->ui->telemetryDetails->height()
                - d->ui->rawTelemetryButton->height()
                - style()->pixelMetric(QStyle::PM_LayoutBottomMargin)
        );
    }
    return QWidget::eventFilter(receiver, event);
}

Provider::TelemetryMode FeedbackConfigWidget::telemetryMode() const
{
    return d->controller->telemetryIndexToMode(d->ui->telemetrySlider->value());
}

int FeedbackConfigWidget::surveyInterval() const
{
    return d->controller->surveyIndexToInterval(d->ui->surveySlider->value());
}

#include "moc_feedbackconfigwidget.cpp"
