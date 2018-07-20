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

    connect(ui->logEntryBox, SIGNAL(activated(int)), this, SLOT(logEntrySelected()));

    auto clearButton = ui->buttonBox->button(QDialogButtonBox::Discard);
    Q_ASSERT(clearButton);
    clearButton->setText(tr("Delete Log"));
    connect(clearButton, SIGNAL(clicked()), this, SLOT(close()));

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
    connect(clearButton, SIGNAL(clicked()), controller, SLOT(clear()));

    setEnabled(true);
}

void AuditLogBrowserDialog::logEntrySelected()
{
    const auto dt = ui->logEntryBox->currentData().toDateTime();
    ui->logEntryView->setText(m_controller->logEntry(dt));
}
