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
