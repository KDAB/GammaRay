/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KUSERFEEDBACK_AUDITLOGBROWSERDIALOG_H
#define KUSERFEEDBACK_AUDITLOGBROWSERDIALOG_H

#include <QDialog>

#include <memory>

namespace KUserFeedback {

namespace Ui
{
class AuditLogBrowserDialog;
}
class AuditLogUiController;

/** Dialog to browse audit log entries. */
class AuditLogBrowserDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AuditLogBrowserDialog(QWidget *parent);
    ~AuditLogBrowserDialog();

    void setUiController(AuditLogUiController *controller);

private Q_SLOTS:
    void logEntrySelected();

private:
    std::unique_ptr<Ui::AuditLogBrowserDialog> ui;
    AuditLogUiController *m_controller;
};

}

#endif // KUSERFEEDBACK_AUDITLOGBROWSERDIALOG_H
