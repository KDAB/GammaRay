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

#ifndef KUSERFEEDBACK_PROVIDER_P_H
#define KUSERFEEDBACK_PROVIDER_P_H

#include "provider.h"

#include <common/surveytargetexpressionevaluator.h>

#include <QDateTime>
#include <QStringList>
#include <QTime>
#include <QTimer>
#include <QVector>

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

    QTime startTime;
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
