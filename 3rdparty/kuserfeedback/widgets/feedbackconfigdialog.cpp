/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "feedbackconfigdialog.h"
#include "ui_feedbackconfigdialog.h"

#include <provider.h>

#include <QDebug>
#include <QPushButton>

using namespace KUserFeedback;

namespace KUserFeedback {
class FeedbackConfigDialogPrivate {
public:
    void updateButtonState();

    std::unique_ptr<Ui::FeedbackConfigDialog> ui;
};
}

FeedbackConfigDialog::FeedbackConfigDialog(QWidget *parent) :
    QDialog(parent),
    d(new FeedbackConfigDialogPrivate)
{
    d->ui.reset(new Ui::FeedbackConfigDialog);
    d->ui->setupUi(this);
    d->ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Contribute!"));
    d->ui->buttonBox->button(QDialogButtonBox::Close)->setText(tr("No, I do not want to contribute."));
}

FeedbackConfigDialog::~FeedbackConfigDialog()
{
}

void FeedbackConfigDialog::setFeedbackProvider(Provider* provider)
{
    d->ui->configWidget->setFeedbackProvider(provider);
    connect(d->ui->configWidget, &FeedbackConfigWidget::configurationChanged, this, [this]() { d->updateButtonState(); });
    d->updateButtonState();
}

void FeedbackConfigDialog::accept()
{
    auto p = d->ui->configWidget->feedbackProvider();
    p->setTelemetryMode(d->ui->configWidget->telemetryMode());
    p->setSurveyInterval(d->ui->configWidget->surveyInterval());
    QDialog::accept();
}

void FeedbackConfigDialogPrivate::updateButtonState()
{
    const auto any = ui->configWidget->surveyInterval() >= 0
        || ui->configWidget->telemetryMode() != Provider::NoTelemetry;

    ui->buttonBox->button(QDialogButtonBox::Ok)->setVisible(any);
    ui->buttonBox->button(QDialogButtonBox::Close)->setVisible(!any);
}

#include "moc_feedbackconfigdialog.cpp"
