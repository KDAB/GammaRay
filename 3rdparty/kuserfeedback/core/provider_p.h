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
    void scheduleNextSubmission();
    void submit(const QUrl &url);
    void submitFinished();

    bool selectSurvey(const SurveyInfo &survey) const;

    Provider::TelemetryMode highestTelemetryMode() const;
    void scheduleEncouragement();
    void emitShowEncouragementMessage();

    void writeAuditLog(const QDateTime &dt);

    QVariant sourceData(const QString &sourceName) const override;

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

    QVector<AbstractDataSource*> dataSources;
};
}

#endif
