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

#include "feedbackconfigwidget.h"
#include "ui_feedbackconfigwidget.h"
#include "auditlogbrowserdialog.h"

#include <abstractdatasource.h>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <auditloguicontroller.h>
#endif
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    AuditLogUiController *auditLogController;
#endif
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

    connect(d->ui->telemetrySlider, SIGNAL(valueChanged(int)), this, SLOT(telemetrySliderChanged()));
    connect(d->ui->telemetrySlider, SIGNAL(valueChanged(int)), this, SIGNAL(configurationChanged()));
    connect(d->ui->surveySlider, SIGNAL(valueChanged(int)), this, SLOT(surveySliderChanged()));
    connect(d->ui->surveySlider, SIGNAL(valueChanged(int)), this, SIGNAL(configurationChanged()));

    d->ui->rawTelemetryButton->setParent(d->ui->telemetryDetails);
    d->ui->rawTelemetryButton->setIcon(style()->standardPixmap(QStyle::SP_DialogHelpButton));
    d->ui->telemetryDetails->installEventFilter(this);
    connect(d->ui->rawTelemetryButton, SIGNAL(toggled(bool)), this, SLOT(telemetrySliderChanged()));

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
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
#else
    d->ui->auditLogLabel->hide();
#endif

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
