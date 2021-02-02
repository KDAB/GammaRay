/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KUSERFEEDBACK_AUDITLOGUICONTROLLER_H
#define KUSERFEEDBACK_AUDITLOGUICONTROLLER_H

#include "kuserfeedbackcore_export.h"

#include <QAbstractItemModel>
#include <QObject>

#include <memory>

QT_BEGIN_NAMESPACE
class QDateTime;
QT_END_NAMESPACE

namespace KUserFeedback {

class AuditLogUiControllerPrivate;

/** Widget/QtQuick-independent logic and data for the audit log browser UI. */
class KUSERFEEDBACKCORE_EXPORT AuditLogUiController : public QObject
{
    Q_OBJECT
    /** Returns a model listing all log entries.
     *  @see logEntryModel()
     */
    Q_PROPERTY(QAbstractItemModel* logEntryModel READ logEntryModel CONSTANT)
    /** Returns @c true if there are log entires to display. */
    Q_PROPERTY(bool hasLogEntries READ hasLogEntries NOTIFY logEntryCountChanged)
public:
    AuditLogUiController(QObject *parent = nullptr);
    ~AuditLogUiController();

    /** Returns @c true if there are log entires to display. */
    bool hasLogEntries() const;

    /** Returns a model listing all telemetry data submission events.
     *  Qt::UserRole contains the timestamp of the log entry for use in logEntry().
     */
    QAbstractItemModel* logEntryModel() const;

    /** Returns a formatted text for display of the specified log entry. */
    Q_INVOKABLE QString logEntry(const QDateTime &dt) const;

public Q_SLOTS:
    /** Delete all audit log entries. */
    void clear();

Q_SIGNALS:
    /** Change notification for the hasLogEntries property. */
    void logEntryCountChanged();

private:
    std::unique_ptr<AuditLogUiControllerPrivate> d;
};

}

#endif // KUSERFEEDBACK_AUDITLOGUICONTROLLER_H
