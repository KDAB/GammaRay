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

#include "auditloguicontroller.h"

#include "provider.h"

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

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    QHash<int, QByteArray> roleNames() const override;
#endif

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

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
QHash<int, QByteArray> AuditLogEntryModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, "display");
    roles.insert(Qt::UserRole, "data");
    return roles;
}
#endif


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
