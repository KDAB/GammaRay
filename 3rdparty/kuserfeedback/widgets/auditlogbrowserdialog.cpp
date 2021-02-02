/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
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
