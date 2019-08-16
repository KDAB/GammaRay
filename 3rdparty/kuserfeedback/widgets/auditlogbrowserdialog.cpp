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

#include "auditlogbrowserdialog.h"
#include "ui_auditlogbrowserdialog.h"

#include <auditloguicontroller.h>

#include <QDateTime>
#include <QPushButton>

using namespace KUserFeedback;

AuditLogBrowserDialog::AuditLogBrowserDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AuditLogBrowserDialog)
    , m_controller(nullptr)
{
    ui->setupUi(this);

    connect(ui->logEntryBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &AuditLogBrowserDialog::logEntrySelected);

    auto clearButton = ui->buttonBox->button(QDialogButtonBox::Discard);
    Q_ASSERT(clearButton);
    clearButton->setText(tr("Delete Log"));
    connect(clearButton, &QPushButton::clicked, this, &AuditLogBrowserDialog::close);

    setEnabled(false);
}

AuditLogBrowserDialog::~AuditLogBrowserDialog()
{
}

void AuditLogBrowserDialog::setUiController(AuditLogUiController *controller)
{
    Q_ASSERT(controller);
    m_controller = controller;
    ui->logEntryBox->setModel(controller->logEntryModel());
    logEntrySelected();

    auto clearButton = ui->buttonBox->button(QDialogButtonBox::Discard);
    connect(clearButton, &QPushButton::clicked, controller, &AuditLogUiController::clear);

    setEnabled(true);
}

void AuditLogBrowserDialog::logEntrySelected()
{
    const auto dt = ui->logEntryBox->currentData().toDateTime();
    ui->logEntryView->setText(m_controller->logEntry(dt));
}
