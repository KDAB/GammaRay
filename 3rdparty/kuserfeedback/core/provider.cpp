/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

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

#include <kuserfeedback_version.h>

#include "logging_p.h"
#include "provider.h"
#include "provider_p.h"
#include "abstractdatasource.h"
#include "startcountsource.h"
#include "surveyinfo.h"
#include "usagetimesource.h"

#include <common/surveytargetexpressionparser.h>
#include <common/surveytargetexpressionevaluator.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QMetaEnum>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSettings>
#include <QUrl>
#include <QUuid>

#include <algorithm>
#include <numeric>

namespace KUserFeedback {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
Q_LOGGING_CATEGORY(Log, "org.kde.UserFeedback", QtInfoMsg)
#else
Q_LOGGING_CATEGORY(Log, "org.kde.UserFeedback")
#endif
}

using namespace KUserFeedback;

ProviderPrivate::ProviderPrivate(Provider *qq)
    : q(qq)
    , networkAccessManager(nullptr)
    , redirectCount(0)
    , submissionInterval(-1)
    , telemetryMode(Provider::NoTelemetry)
    , surveyInterval(-1)
    , startCount(0)
    , usageTime(0)
    , encouragementStarts(-1)
    , encouragementTime(-1)
    , encouragementDelay(300)
    , encouragementInterval(-1)
    , backoffIntervalMinutes(-1)
{
    submissionTimer.setSingleShot(true);
    QObject::connect(&submissionTimer, &QTimer::timeout, q, &Provider::submit);

    startTime.start();

    encouragementTimer.setSingleShot(true);
    QObject::connect(&encouragementTimer, &QTimer::timeout, q, [this]() { emitShowEncouragementMessage(); });
}

ProviderPrivate::~ProviderPrivate()
{
    qDeleteAll(dataSources);
}

int ProviderPrivate::currentApplicationTime() const
{
    return usageTime + (startTime.elapsed() / 1000);
}

static QMetaEnum telemetryModeEnum()
{
    const auto idx = Provider::staticMetaObject.indexOfEnumerator("TelemetryMode");
    Q_ASSERT(idx >= 0);
    return Provider::staticMetaObject.enumerator(idx);
}

std::unique_ptr<QSettings> ProviderPrivate::makeSettings() const
{
    // attempt to put our settings next to the application ones,
    // so replicate how QSettings handles this
    auto org =
#ifdef Q_OS_MAC
        QCoreApplication::organizationDomain().isEmpty() ? QCoreApplication::organizationName() : QCoreApplication::organizationDomain();
#else
        QCoreApplication::organizationName().isEmpty() ? QCoreApplication::organizationDomain() : QCoreApplication::organizationName();
#endif
    if (org.isEmpty())
        org = QLatin1String("Unknown Organization");

    std::unique_ptr<QSettings> s(new QSettings(org, QStringLiteral("UserFeedback.") + productId));
    return s;
}

std::unique_ptr<QSettings> ProviderPrivate::makeGlobalSettings() const
{
    const auto org =
#ifdef Q_OS_MAC
        QStringLiteral("kde.org");
#else
        QStringLiteral("KDE");
#endif
    std::unique_ptr<QSettings> s(new QSettings(org, QStringLiteral("UserFeedback")));
    return s;
}

void ProviderPrivate::load()
{
    auto s = makeSettings();
    s->beginGroup(QStringLiteral("UserFeedback"));
    lastSubmitTime = s->value(QStringLiteral("LastSubmission")).toDateTime();

    const auto modeStr = s->value(QStringLiteral("StatisticsCollectionMode")).toByteArray();
    telemetryMode = static_cast<Provider::TelemetryMode>(std::max(telemetryModeEnum().keyToValue(modeStr.constData()), 0));

    surveyInterval = s->value(QStringLiteral("SurveyInterval"), -1).toInt();
    lastSurveyTime = s->value(QStringLiteral("LastSurvey")).toDateTime();
    completedSurveys = s->value(QStringLiteral("CompletedSurveys"), QStringList()).toStringList();

    startCount = std::max(s->value(QStringLiteral("ApplicationStartCount"), 0).toInt(), 0);
    usageTime = std::max(s->value(QStringLiteral("ApplicationTime"), 0).toInt(), 0);

    lastEncouragementTime = s->value(QStringLiteral("LastEncouragement")).toDateTime();

    s->endGroup();

    foreach (auto source, dataSources) {
        s->beginGroup(QStringLiteral("Source-") + source->id());
        source->load(s.get());
        s->endGroup();
    }

    auto g = makeGlobalSettings();
    g->beginGroup(QStringLiteral("UserFeedback"));
    lastSurveyTime = std::max(g->value(QStringLiteral("LastSurvey")).toDateTime(), lastSurveyTime);
    lastEncouragementTime = std::max(g->value(QStringLiteral("LastEncouragement")).toDateTime(), lastEncouragementTime);
}

void ProviderPrivate::store()
{
    auto s = makeSettings();
    s->beginGroup(QStringLiteral("UserFeedback"));

    // another process might have changed this, so read the base value first before writing
    usageTime = std::max(s->value(QStringLiteral("ApplicationTime"), 0).toInt(), usageTime);
    s->setValue(QStringLiteral("ApplicationTime"), currentApplicationTime());
    usageTime = currentApplicationTime();
    startTime.restart();

    s->endGroup();

    foreach (auto source, dataSources) {
        s->beginGroup(QStringLiteral("Source-") + source->id());
        source->store(s.get());
        s->endGroup();
    }
}

void ProviderPrivate::storeOne(const QString &key, const QVariant &value)
{
    auto s = makeSettings();
    s->beginGroup(QStringLiteral("UserFeedback"));
    s->setValue(key, value);
}

void ProviderPrivate::storeOneGlobal(const QString &key, const QVariant &value)
{
    auto s = makeGlobalSettings();
    s->beginGroup(QStringLiteral("UserFeedback"));
    s->setValue(key, value);
}

void ProviderPrivate::aboutToQuit()
{
    store();
}

bool ProviderPrivate::isValidSource(AbstractDataSource *source) const
{
    if (source->id().isEmpty()) {
        qCWarning(Log) << "Skipping data source with empty name!";
        return false;
    }
    if (source->telemetryMode() == Provider::NoTelemetry) {
        qCWarning(Log) << "Source" << source->id() << "attempts to report data unconditionally, ignoring!";
        return false;
    }
    if (source->description().isEmpty()) {
        qCWarning(Log) << "Source" << source->id() << "has no description, ignoring!";
        return false;
    }

    Q_ASSERT(!source->id().isEmpty());
    Q_ASSERT(source->telemetryMode() != Provider::NoTelemetry);
    Q_ASSERT(!source->description().isEmpty());
    return true;
}

QByteArray ProviderPrivate::jsonData(Provider::TelemetryMode mode) const
{
    QJsonObject obj;
    if (mode != Provider::NoTelemetry) {
        foreach (auto source, dataSources) {
            if (!isValidSource(source) || !source->isActive())
                continue;
            if (mode < source->telemetryMode())
                continue;
            const auto data = source->data();
            if (data.canConvert<QVariantMap>())
                obj.insert(source->id(), QJsonObject::fromVariantMap(data.toMap()));
            else if (data.canConvert<QVariantList>())
                obj.insert(source->id(), QJsonArray::fromVariantList(data.value<QVariantList>()));
            else
                qCWarning(Log) << "wrong type for" << source->id() << data;
        }
    }

    QJsonDocument doc(obj);
    return doc.toJson();
}

void ProviderPrivate::scheduleNextSubmission(qint64 minTime)
{
    submissionTimer.stop();
    if (!q->isEnabled())
        return;
    if (submissionInterval <= 0 || (telemetryMode == Provider::NoTelemetry && surveyInterval < 0))
        return;

    if (minTime == 0) {
        // If this is a regularly scheduled submission reset the backoff
        backoffIntervalMinutes = -1;
    }

    Q_ASSERT(submissionInterval > 0);

    const auto nextSubmission = lastSubmitTime.addDays(submissionInterval);
    const auto now = QDateTime::currentDateTime();
    submissionTimer.start(std::max(minTime, now.msecsTo(nextSubmission)));
}

void ProviderPrivate::submitFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        if (backoffIntervalMinutes == -1) {
            backoffIntervalMinutes = 2;
        } else {
            backoffIntervalMinutes = backoffIntervalMinutes * 2;
        }
        qCWarning(Log) << "failed to submit user feedback:" << reply->errorString() << reply->readAll() << ". Calling scheduleNextSubmission with minTime" << backoffIntervalMinutes << "minutes";
        scheduleNextSubmission(backoffIntervalMinutes * 60000ll);
        return;
    }

    const auto redirectTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (redirectTarget.isValid()) {
        if (++redirectCount >= 20) {
            qCWarning(Log) << "Redirect loop on" << reply->url().resolved(redirectTarget).toString();
            return;
        }
        submit(reply->url().resolved(redirectTarget));
        return;
    }

    lastSubmitTime = QDateTime::currentDateTime();

    auto s = makeSettings();
    s->beginGroup(QStringLiteral("UserFeedback"));
    s->setValue(QStringLiteral("LastSubmission"), lastSubmitTime);
    s->endGroup();

    writeAuditLog(lastSubmitTime);

    // reset source counters
    foreach (auto source, dataSources) {
        s->beginGroup(QStringLiteral("Source-") + source->id());
        source->reset(s.get());
        s->endGroup();
    }

    const auto obj = QJsonDocument::fromJson(reply->readAll()).object();
    const auto it = obj.find(QLatin1String("surveys"));
    if (it != obj.end() && surveyInterval >= 0) {
        const auto a = it.value().toArray();
        qCDebug(Log) << "received" << a.size() << "surveys";
        foreach(const auto &s, a) {
            const auto survey = SurveyInfo::fromJson(s.toObject());
            if (selectSurvey(survey))
                break;
        }
    }

    scheduleNextSubmission();
}

QVariant ProviderPrivate::sourceData(const QString& sourceId) const
{
    foreach (auto src, dataSources) {
        if (src->id() == sourceId)
            return src->data();
    }
    return QVariant();
}

bool ProviderPrivate::selectSurvey(const SurveyInfo &survey) const
{
    qCDebug(Log) << "got survey:" << survey.url() << survey.target();
    if (!q->isEnabled() || !survey.isValid() || completedSurveys.contains(survey.uuid().toString()))
        return false;

    if (surveyInterval != 0 && lastSurveyTime.addDays(surveyInterval) > QDateTime::currentDateTime())
        return false;

    if (!survey.target().isEmpty()) {
        SurveyTargetExpressionParser parser;
        if (!parser.parse(survey.target())) {
            qCDebug(Log) << "failed to parse target expression";
            return false;
        }

        SurveyTargetExpressionEvaluator eval;
        eval.setDataProvider(this);
        if (!eval.evaluate(parser.expression()))
            return false;
    }

    qCDebug(Log) << "picked survey:" << survey.url();
    emit q->surveyAvailable(survey);
    return true;
}

Provider::TelemetryMode ProviderPrivate::highestTelemetryMode() const
{
    auto mode = Provider::NoTelemetry;
    foreach (auto src, dataSources)
        mode = std::max(mode, src->telemetryMode());
    return mode;
}

void ProviderPrivate::scheduleEncouragement()
{
    encouragementTimer.stop();
    if (!q->isEnabled())
        return;

    // already done, not repetition
    if (lastEncouragementTime.isValid() && encouragementInterval <= 0)
        return;

    if (encouragementStarts < 0 && encouragementTime < 0) // encouragement disabled
        return;

    if (encouragementStarts > startCount) // we need more starts
        return;

    if (telemetryMode >= highestTelemetryMode() && surveyInterval == 0) // already everything enabled
        return;
    // no repetition if some feedback is enabled
    if (lastEncouragementTime.isValid() && (telemetryMode > Provider::NoTelemetry || surveyInterval >= 0))
        return;

    Q_ASSERT(encouragementDelay >= 0);
    int timeToEncouragement = encouragementDelay;
    if (encouragementTime > 0)
        timeToEncouragement = std::max(timeToEncouragement, encouragementTime - currentApplicationTime());
    if (lastEncouragementTime.isValid()) {
        Q_ASSERT(encouragementInterval > 0);
        const auto targetTime = lastEncouragementTime.addDays(encouragementInterval);
        timeToEncouragement = std::max(timeToEncouragement, (int)QDateTime::currentDateTime().secsTo(targetTime));
    }
    encouragementTimer.start(timeToEncouragement * 1000);
}

void ProviderPrivate::emitShowEncouragementMessage()
{
    lastEncouragementTime = QDateTime::currentDateTime(); // TODO make this explicit, in case the host application decides to delay?
    storeOne(QStringLiteral("LastEncouragement"), lastEncouragementTime);
    storeOneGlobal(QStringLiteral("LastEncouragement"), lastEncouragementTime);
    emit q->showEncouragementMessage();
}


Provider::Provider(QObject *parent) :
    QObject(parent),
    d(new ProviderPrivate(this))
{
    qCDebug(Log);

    connect(QCoreApplication::instance(), &QCoreApplication::aboutToQuit, this, [this]() { d->aboutToQuit(); });

    auto domain = QCoreApplication::organizationDomain().split(QLatin1Char('.'));
    std::reverse(domain.begin(), domain.end());
    auto id = domain.join(QLatin1String("."));
    if (!id.isEmpty())
        id += QLatin1Char('.');
    id += QCoreApplication::applicationName();
    setProductIdentifier(id);
}

Provider::~Provider()
{
    delete d;
}

bool Provider::isEnabled() const
{
    auto s = d->makeGlobalSettings();
    s->beginGroup(QStringLiteral("UserFeedback"));
    return s->value(QStringLiteral("Enabled"), true).toBool();
}

void Provider::setEnabled(bool enabled)
{
    if (enabled == isEnabled())
        return;
    d->storeOneGlobal(QStringLiteral("Enabled"), enabled);
    emit enabledChanged();
}

QString Provider::productIdentifier() const
{
    return d->productId;
}

void Provider::setProductIdentifier(const QString &productId)
{
    Q_ASSERT(!productId.isEmpty());
    if (productId == d->productId)
        return;
    d->productId = productId;

    d->load();
    d->startCount++;
    d->storeOne(QStringLiteral("ApplicationStartCount"), d->startCount);

    emit providerSettingsChanged();

    d->scheduleEncouragement();
    d->scheduleNextSubmission();
}

QUrl Provider::feedbackServer() const
{
    return d->serverUrl;
}

void Provider::setFeedbackServer(const QUrl &url)
{
    if (d->serverUrl == url)
        return;
    d->serverUrl = url;
    emit providerSettingsChanged();
}

int Provider::submissionInterval() const
{
    return d->submissionInterval;
}

void Provider::setSubmissionInterval(int days)
{
    if (d->submissionInterval == days)
        return;
    d->submissionInterval = days;
    emit providerSettingsChanged();
    d->scheduleNextSubmission();
}

Provider::TelemetryMode Provider::telemetryMode() const
{
    return d->telemetryMode;
}

void Provider::setTelemetryMode(TelemetryMode mode)
{
    if (d->telemetryMode == mode)
        return;

    d->telemetryMode = mode;
    d->storeOne(QStringLiteral("StatisticsCollectionMode"), QString::fromLatin1(telemetryModeEnum().valueToKey(d->telemetryMode)));
    d->scheduleNextSubmission();
    d->scheduleEncouragement();
    emit telemetryModeChanged();
}

void Provider::addDataSource(AbstractDataSource *source)
{
    // special cases for sources where we track the data here, as it's needed even if we don't report it
    if (auto countSrc = dynamic_cast<StartCountSource*>(source))
        countSrc->setProvider(d);
    if (auto timeSrc = dynamic_cast<UsageTimeSource*>(source))
        timeSrc->setProvider(d);

    d->dataSources.push_back(source);
    d->dataSourcesById[source->id()] = source;

    auto s = d->makeSettings();
    s->beginGroup(QStringLiteral("Source-") + source->id());
    source->load(s.get());
}

QVector<AbstractDataSource*> Provider::dataSources() const
{
    return d->dataSources;
}

AbstractDataSource *Provider::dataSource(const QString &id) const
{
    auto it = d->dataSourcesById.find(id);
    return it != std::end(d->dataSourcesById) ? *it : nullptr;
}

int Provider::surveyInterval() const
{
    return d->surveyInterval;
}

void Provider::setSurveyInterval(int days)
{
    if (d->surveyInterval == days)
        return;

    d->surveyInterval = days;
    d->storeOne(QStringLiteral("SurveyInterval"), d->surveyInterval);

    d->scheduleNextSubmission();
    d->scheduleEncouragement();
    emit surveyIntervalChanged();
}

int Provider::applicationStartsUntilEncouragement() const
{
    return d->encouragementStarts;
}

void Provider::setApplicationStartsUntilEncouragement(int starts)
{
    if (d->encouragementStarts == starts)
        return;
    d->encouragementStarts = starts;
    emit providerSettingsChanged();
    d->scheduleEncouragement();
}

int Provider::applicationUsageTimeUntilEncouragement() const
{
    return d->encouragementTime;
}

void Provider::setApplicationUsageTimeUntilEncouragement(int secs)
{
    if (d->encouragementTime == secs)
        return;
    d->encouragementTime = secs;
    emit providerSettingsChanged();
    d->scheduleEncouragement();
}

int Provider::encouragementDelay() const
{
    return d->encouragementDelay;
}

void Provider::setEncouragementDelay(int secs)
{
    if (d->encouragementDelay == secs)
        return;
    d->encouragementDelay = std::max(0, secs);
    emit providerSettingsChanged();
    d->scheduleEncouragement();
}

int Provider::encouragementInterval() const
{
    return d->encouragementInterval;
}

void Provider::setEncouragementInterval(int days)
{
    if (d->encouragementInterval == days)
        return;
    d->encouragementInterval = days;
    emit providerSettingsChanged();
    d->scheduleEncouragement();
}

void Provider::surveyCompleted(const SurveyInfo &info)
{
    d->completedSurveys.push_back(info.uuid().toString());
    d->lastSurveyTime = QDateTime::currentDateTime();

    auto s = d->makeSettings();
    s->beginGroup(QStringLiteral("UserFeedback"));
    s->setValue(QStringLiteral("LastSurvey"), d->lastSurveyTime);
    s->setValue(QStringLiteral("CompletedSurveys"), d->completedSurveys);

    d->storeOneGlobal(QStringLiteral("LastSurvey"), d->lastSurveyTime);
}

void Provider::load()
{
    d->load();
}

void Provider::store()
{
    d->store();
}

void Provider::submit()
{
    if (!isEnabled()) {
        qCWarning(Log) << "Global kill switch is enabled";
        return;
    }
    if (d->productId.isEmpty()) {
        qCWarning(Log) << "No productId specified!";
        return;
    }
    if (!d->serverUrl.isValid()) {
        qCWarning(Log) << "No feedback server URL specified!";
        return;
    }

    if (!d->networkAccessManager)
        d->networkAccessManager = new QNetworkAccessManager(this);

    auto url = d->serverUrl;
    auto path = d->serverUrl.path();
    if (!path.endsWith(QLatin1Char('/')))
        path += QLatin1Char('/');
    path += QStringLiteral("receiver/submit/") + d->productId;
    url.setPath(path);
    d->submitProbe(url);
}

void ProviderPrivate::submit(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));
    request.setHeader(QNetworkRequest::UserAgentHeader, QString(QStringLiteral("KUserFeedback/") + QStringLiteral(KUSERFEEDBACK_VERSION_STRING)));
    auto reply = networkAccessManager->post(request, jsonData(telemetryMode));
    QObject::connect(reply, &QNetworkReply::finished, q, [this, reply]() { submitFinished(reply); });
}

void ProviderPrivate::submitProbe(const QUrl &url)
{
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, QString(QStringLiteral("KUserFeedback/") + QStringLiteral(KUSERFEEDBACK_VERSION_STRING)));
    auto reply = networkAccessManager->get(request);
    QObject::connect(reply, &QNetworkReply::finished, q, [this, reply]() { submitProbeFinished(reply); });
}

void ProviderPrivate::submitProbeFinished(QNetworkReply *reply)
{
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError) {
        qCWarning(Log) << "failed to probe user feedback submission interface:" << reply->errorString() << reply->readAll();
        return;
    }

    const auto redirectTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();
    if (redirectTarget.isValid()) {
        if (++redirectCount >= 20) {
            qCWarning(Log) << "Redirect loop on" << reply->url().resolved(redirectTarget).toString();
            return;
        }
        submitProbe(reply->url().resolved(redirectTarget));
        return;
    }

    submit(reply->url());
}

void ProviderPrivate::writeAuditLog(const QDateTime &dt)
{
    const QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + QStringLiteral("/kuserfeedback/audit");
    QDir().mkpath(path);

    QJsonObject docObj;
    foreach (auto source, dataSources) {
        if (!isValidSource(source) || !source->isActive() || telemetryMode < source->telemetryMode())
            continue;
        QJsonObject obj;
        const auto data = source->data();
        if (data.canConvert<QVariantMap>())
            obj.insert(QLatin1String("data"), QJsonObject::fromVariantMap(data.toMap()));
        else if (data.canConvert<QVariantList>())
            obj.insert(QLatin1String("data"), QJsonArray::fromVariantList(data.value<QVariantList>()));
        if (obj.isEmpty())
            continue;
        obj.insert(QLatin1String("telemetryMode"), QString::fromLatin1(telemetryModeEnum().valueToKey(source->telemetryMode())));
        obj.insert(QLatin1String("description"), source->description());
        docObj.insert(source->id(), obj);
    }

    QFile file(path + QLatin1Char('/') + dt.toString(QStringLiteral("yyyyMMdd-hhmmss")) + QStringLiteral(".log"));
    if (!file.open(QFile::WriteOnly)) {
        qCWarning(Log) << "Unable to open audit log file:" << file.fileName() << file.errorString();
        return;
    }

    QJsonDocument doc(docObj);
    file.write(doc.toJson());

    qCDebug(Log) << "Audit log written:" << file.fileName();
}

#include "moc_provider.cpp"
