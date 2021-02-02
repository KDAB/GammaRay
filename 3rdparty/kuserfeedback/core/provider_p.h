/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#ifndef KUSERFEEDBACK_PROVIDER_P_H
#define KUSERFEEDBACK_PROVIDER_P_H

#include "provider.h"

#include <common/surveytargetexpressionevaluator.h>

#include <QDateTime>
#include <QStringList>
#include <QTime>
#include <QTimer>
#include <QVector>
#include <QElapsedTimer>

#include <memory>

QT_BEGIN_NAMESPACE
class QNetworkAccessManager;
class QNetworkReply;
class QSettings;
QT_END_NAMESPACE

namespace KUserFeedback {
class ProviderPrivate : public SurveyTargetExpressionDataProvider
{
public:
    explicit ProviderPrivate(Provider *qq);
    ~ProviderPrivate();

    int currentApplicationTime() const;

    std::unique_ptr<QSettings> makeSettings() const;
    std::unique_ptr<QSettings> makeGlobalSettings() const;
    void load();
    void store();
    void storeOne(const QString &key, const QVariant &value);
    void storeOneGlobal(const QString &key, const QVariant &value);

    void aboutToQuit();

    bool isValidSource(AbstractDataSource *source) const;
    QByteArray jsonData(Provider::TelemetryMode mode) const;
    void scheduleNextSubmission(qint64 minTime = 0);
    void submitProbe(const QUrl &url);
    void submitProbeFinished(QNetworkReply *reply);
    void submit(const QUrl &url);
    void submitFinished(QNetworkReply *reply);

    bool selectSurvey(const SurveyInfo &survey) const;

    Provider::TelemetryMode highestTelemetryMode() const;
    void scheduleEncouragement();
    void emitShowEncouragementMessage();

    void writeAuditLog(const QDateTime &dt);

    QVariant sourceData(const QString &sourceId) const override;

    Provider *q;

    QString productId;

    QTimer submissionTimer;
    QNetworkAccessManager *networkAccessManager;
    QUrl serverUrl;
    QDateTime lastSubmitTime;
    int redirectCount;
    int submissionInterval;
    Provider::TelemetryMode telemetryMode;

    int surveyInterval;
    QDateTime lastSurveyTime;
    QStringList completedSurveys;

    QElapsedTimer startTime;
    int startCount;
    int usageTime;

    QTimer encouragementTimer;
    QDateTime lastEncouragementTime;
    int encouragementStarts;
    int encouragementTime;
    int encouragementDelay;
    int encouragementInterval;

    int backoffIntervalMinutes;

    QVector<AbstractDataSource*> dataSources;
    QHash<QString, AbstractDataSource*> dataSourcesById;
};
}

#endif
