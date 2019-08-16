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
