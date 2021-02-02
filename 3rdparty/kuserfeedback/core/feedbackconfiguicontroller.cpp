/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "feedbackconfiguicontroller.h"
#include "abstractdatasource.h"

#include <QGuiApplication>
#include <QSet>
#include <QVector>

#include <algorithm>
#include <vector>

using namespace KUserFeedback;

namespace KUserFeedback {
class FeedbackConfigUiControllerPrivate {
public:
    FeedbackConfigUiControllerPrivate();

    Provider *provider;
    std::vector<Provider::TelemetryMode> telemetryModeMap;
    QString m_appName;
};
}

FeedbackConfigUiControllerPrivate::FeedbackConfigUiControllerPrivate() :
    provider(nullptr),
    m_appName(QGuiApplication::applicationDisplayName())
{
}

FeedbackConfigUiController::FeedbackConfigUiController(QObject* parent)
    : QObject(parent)
    , d(new FeedbackConfigUiControllerPrivate)
{
}

FeedbackConfigUiController::~FeedbackConfigUiController()
{
}

Provider* FeedbackConfigUiController::feedbackProvider() const
{
    return d->provider;
}

void FeedbackConfigUiController::setFeedbackProvider(Provider* provider)
{
    if (d->provider == provider)
        return;
    d->provider = provider;

    d->telemetryModeMap.clear();
    d->telemetryModeMap.reserve(5);
    d->telemetryModeMap.push_back(Provider::NoTelemetry);
    d->telemetryModeMap.push_back(Provider::BasicSystemInformation);
    d->telemetryModeMap.push_back(Provider::BasicUsageStatistics);
    d->telemetryModeMap.push_back(Provider::DetailedSystemInformation);
    d->telemetryModeMap.push_back(Provider::DetailedUsageStatistics);

    QSet<Provider::TelemetryMode> supportedModes;
    supportedModes.reserve(d->telemetryModeMap.size());
    supportedModes.insert(Provider::NoTelemetry);
    foreach (const auto &src, provider->dataSources())
        supportedModes.insert(src->telemetryMode());
    for (auto it = d->telemetryModeMap.begin(); it != d->telemetryModeMap.end();) {
        if (!supportedModes.contains(*it))
            it = d->telemetryModeMap.erase(it);
        else
            ++it;
    }

    emit providerChanged();
}

int FeedbackConfigUiController::telemetryModeCount() const
{
    return d->telemetryModeMap.size();
}

int FeedbackConfigUiController::surveyModeCount() const
{
    return 3;
}

Provider::TelemetryMode FeedbackConfigUiController::telemetryIndexToMode(int index) const
{
    if (index < 0 || index >= telemetryModeCount())
        return Provider::NoTelemetry;
    return d->telemetryModeMap[index];
}

int FeedbackConfigUiController::telemetryModeToIndex(Provider::TelemetryMode mode) const
{
    const auto it = std::lower_bound(d->telemetryModeMap.begin(), d->telemetryModeMap.end(), mode);
    if (it == d->telemetryModeMap.end())
        return 0;
    return std::distance(d->telemetryModeMap.begin(), it);
}

QString FeedbackConfigUiController::telemetryModeName(int telemetryIndex) const
{
    return telemetryName(telemetryIndexToMode(telemetryIndex));
}

QString FeedbackConfigUiController::telemetryModeDescription(int telemetryIndex) const
{
    return telemetryDescription(telemetryIndexToMode(telemetryIndex));
}

QString FeedbackConfigUiController::telemetryName(KUserFeedback::Provider::TelemetryMode mode) const
{
    switch (mode) {
        case Provider::NoTelemetry:
            return tr("Disabled");
        case Provider::BasicSystemInformation:
            return tr("Basic system information");
        case Provider::BasicUsageStatistics:
            return tr("Basic system information and usage statistics");
        case Provider::DetailedSystemInformation:
            return tr("Detailed system information and basic usage statistics");
        case Provider::DetailedUsageStatistics:
            return tr("Detailed system information and usage statistics");
    }

    return {};
}

QString FeedbackConfigUiController::telemetryDescription(KUserFeedback::Provider::TelemetryMode mode) const
{
    const auto name = applicationName();
    if (name.isEmpty()) {
        switch (mode) {
            case Provider::NoTelemetry:
                return tr(
                    "Don't share anything"
                );
            case Provider::BasicSystemInformation:
                return tr(
                    "Share basic system information such as the version of the application and the operating system"
                );
            case Provider::BasicUsageStatistics:
                return tr(
                    "Share basic system information and basic statistics on how often you use the application"
                );
            case Provider::DetailedSystemInformation:
                return tr(
                    "Share basic statistics on how often you use the application, as well as more detailed information about your system"
                );
            case Provider::DetailedUsageStatistics:
                return tr(
                    "Share detailed system information and statistics on how often individual features of the application are used."
                );
        }
    } else {
        switch (mode) {
            case Provider::NoTelemetry:
                return tr(
                    "Don't share anything"
                );
            case Provider::BasicSystemInformation:
                return tr(
                    "Share basic system information such as the version of %1 and and the operating system"
                ).arg(name);
            case Provider::BasicUsageStatistics:
                return tr(
                    "Share basic system information and basic statistics on how often you use %1"
                ).arg(name);
            case Provider::DetailedSystemInformation:
                return tr(
                    "Share basic statistics on how often you use %1, as well as more detailed information about your system"
                ).arg(name);
            case Provider::DetailedUsageStatistics:
                return tr(
                    "Share detailed system information and statistics on how often individual features of %1 are used."
                ).arg(name);
        }
    }

    return QString();
}

QString FeedbackConfigUiController::telemetryModeDetails(int telemetryIndex) const
{
    if (telemetryIndex <= 0 || telemetryIndex >= telemetryModeCount())
        return QString();

    auto srcs = d->provider->dataSources();
    std::stable_sort(srcs.begin(), srcs.end(), [](AbstractDataSource *lhs, AbstractDataSource *rhs) {
        return lhs->telemetryMode() < rhs->telemetryMode();
    });

    auto detailsStr = QStringLiteral("<ul>");
    foreach (const auto *src, srcs) {
        if (telemetryIndex >= telemetryModeToIndex(src->telemetryMode()) && !src->description().isEmpty())
            detailsStr += QStringLiteral("<li>") + src->description() + QStringLiteral("</li>");
    }
    return detailsStr + QStringLiteral("</ul>");
}

int FeedbackConfigUiController::surveyIndexToInterval(int index) const
{
    switch (index) {
        case 0: return -1;
        case 1: return 90;
        case 2: return 0;
    }
    return -1;
}

int FeedbackConfigUiController::surveyIntervalToIndex(int interval) const
{
    if (interval < 0)
        return 0;
    else if (interval >= 90)
        return 1;
    else
        return 2;
}

QString FeedbackConfigUiController::surveyModeDescription(int surveyIndex) const
{
    const auto name = applicationName();
    if (name.isEmpty()) {
        switch (surveyIndex) {
            case 0:
                return tr(
                    "Don't participate in usability surveys"
                );
            case 1:
                return tr(
                    "Participate in surveys about the application not more than four times a year"
                );
            case 2:
                return tr(
                    "Participate in surveys about the application whenever one is available (they can be deferred or skipped)"
                );
        }
    } else {
        switch (surveyIndex) {
            case 0:
                return tr(
                    "Don't participate in usability surveys about %1"
                ).arg(name);
            case 1:
                return tr(
                    "Participate in surveys about %1 not more than four times a year"
                ).arg(name);
            case 2:
                return tr(
                    "Participate in surveys about %1 whenever one is available (they can be deferred or skipped)"
                ).arg(name);
        }
    }

    return QString();
}

QString FeedbackConfigUiController::applicationName() const
{
    return d->m_appName;
}

void FeedbackConfigUiController::setApplicationName(const QString& appName)
{
    if (appName == d->m_appName)
        return;

    d->m_appName = appName;
    Q_EMIT applicationNameChanged(appName);
}
