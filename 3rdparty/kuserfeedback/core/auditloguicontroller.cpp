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

#include "auditloguicontroller.h"

#include <provider.h>

#include <QAbstractListModel>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QMetaEnum>
#include <QStandardPaths>

#include <algorithm>
#include <vector>

using namespace KUserFeedback;

namespace KUserFeedback {
class AuditLogEntryModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AuditLogEntryModel(const QString &path, QObject *parent);

    void reload();

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

private:
    QString m_path;
    std::vector<QDateTime> m_entries;
};


class AuditLogUiControllerPrivate
{
public:
    QString path;
    AuditLogEntryModel *logEntryModel;
};
}


AuditLogEntryModel::AuditLogEntryModel(const QString &path, QObject *parent)
    : QAbstractListModel(parent)
    , m_path(path)
{
    reload();
}

void AuditLogEntryModel::reload()
{
    beginResetModel();
    m_entries.clear();

    foreach (auto e, QDir(m_path).entryList(QDir::Files | QDir::Readable)) {
        if (!e.endsWith(QLatin1String(".log")))
            continue;
        e.chop(4);
        const auto dt = QDateTime::fromString(e, QStringLiteral("yyyyMMdd-hhmmss"));
        if (dt.isValid())
            m_entries.push_back(dt);
    }
    std::sort(m_entries.begin(), m_entries.end(), [](const QDateTime &lhs, const QDateTime &rhs) {
        return lhs > rhs;
    });
    endResetModel();
}

int AuditLogEntryModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return m_entries.size();
}

QVariant AuditLogEntryModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
        case Qt::DisplayRole:
            return QLocale().toString(m_entries[index.row()]);
        case Qt::UserRole:
            return m_entries[index.row()];
    }
    return QVariant();
}

QHash<int, QByteArray> AuditLogEntryModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, "display");
    roles.insert(Qt::UserRole, "data");
    return roles;
}


AuditLogUiController::AuditLogUiController(QObject* parent)
    : QObject(parent)
    , d(new AuditLogUiControllerPrivate)
{
    d->path = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QStringLiteral("/kuserfeedback/audit/");
    d->logEntryModel = new AuditLogEntryModel(d->path, this);

    connect(d->logEntryModel, &QAbstractItemModel::modelReset, this, &AuditLogUiController::logEntryCountChanged);
}

AuditLogUiController::~AuditLogUiController()
{
}

bool AuditLogUiController::hasLogEntries() const
{
    return d->logEntryModel->rowCount() != 0;
}

QAbstractItemModel* AuditLogUiController::logEntryModel() const
{
    return d->logEntryModel;
}

static QString telemetryModeString(Provider::TelemetryMode mode)
{
    switch (mode) {
        case Provider::NoTelemetry:
            Q_ASSERT(false);
            return QString();
        case Provider::BasicSystemInformation:
            return AuditLogUiController::tr("Basic System Information");
        case Provider::BasicUsageStatistics:
            return AuditLogUiController::tr("Basic Usage Statistics");
        case Provider::DetailedSystemInformation:
            return AuditLogUiController::tr("Detailed System Information");
        case Provider::DetailedUsageStatistics:
            return AuditLogUiController::tr("Detailed Usage Statistics");
    }
    Q_UNREACHABLE();
}

QString AuditLogUiController::logEntry(const QDateTime &dt) const
{
    const QString fn = d->path + dt.toString(QStringLiteral("yyyyMMdd-hhmmss")) + QStringLiteral(".log");
    QFile file(fn);
    if (!file.open(QFile::ReadOnly))
        return tr("Unable to open file %1: %2.").arg(fn, file.errorString());

    const auto doc = QJsonDocument::fromJson(file.readAll());
    const auto topObj = doc.object();
    struct Entry {
        QString key;
        QString desc;
        QString rawData;
        Provider::TelemetryMode mode;
    };
    std::vector<Entry> entries;
    entries.reserve(topObj.size());

    const auto idx = Provider::staticMetaObject.indexOfEnumerator("TelemetryMode");
    Q_ASSERT(idx >= 0);
    const auto modeEnum = Provider::staticMetaObject.enumerator(idx);

    for (auto it = topObj.begin(); it != topObj.end(); ++it) {
        Entry e;
        e.key = it.key();
        const auto obj = it.value().toObject();
        e.desc = obj.value(QLatin1String("description")).toString();
        const auto data = obj.value(QLatin1String("data"));
        if (data.isObject())
            e.rawData = QString::fromUtf8(QJsonDocument(data.toObject()).toJson());
        else if (data.isArray())
            e.rawData = QString::fromUtf8(QJsonDocument(data.toArray()).toJson());
        e.mode = static_cast<Provider::TelemetryMode>(modeEnum.keyToValue(obj.value(QLatin1String("telemetryMode")).toString().toUtf8().constData()));
        entries.push_back(e);
    }

    std::sort(entries.begin(), entries.end(), [](const Entry &lhs, const Entry &rhs) -> bool {
        if (lhs.mode == rhs.mode)
            return lhs.key < rhs.key;
        return lhs.mode < rhs.mode;
    });

    QString res;
    for (auto it = entries.begin(); it != entries.end(); ++it) {
        res += QStringLiteral("<b>") + (*it).desc + QStringLiteral("</b><br/>");
        res += tr("Category: <i>%1</i><br/>").arg(telemetryModeString((*it).mode));
        res += tr("Key: <i>%1</i><br/>").arg((*it).key);
        res += tr("Submitted data: <tt>%1</tt><br/><br/>").arg((*it).rawData);
    }
    return res;
}

void AuditLogUiController::clear()
{
    QDir dir(d->path);
    foreach (auto e, dir.entryList(QDir::Files | QDir::Readable)) {
        if (!e.endsWith(QLatin1String(".log")))
            continue;
        dir.remove(e);
    }

    d->logEntryModel->reload();
}

#include "auditloguicontroller.moc"
