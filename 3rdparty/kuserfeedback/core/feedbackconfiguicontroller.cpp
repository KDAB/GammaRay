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

#include "feedbackconfiguicontroller.h"
#include "abstractdatasource.h"

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QGuiApplication>
#endif
#include <QSet>
#include <QVector>

#include <algorithm>
#include <vector>

using namespace KUserFeedback;

namespace KUserFeedback {
class FeedbackConfigUiControllerPrivate {
public:
    FeedbackConfigUiControllerPrivate();

    static QString appName();

    Provider *provider;
    std::vector<Provider::TelemetryMode> telemetryModeMap;
};
}

FeedbackConfigUiControllerPrivate::FeedbackConfigUiControllerPrivate() :
    provider(nullptr)
{
}

QString FeedbackConfigUiControllerPrivate::appName()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QGuiApplication::applicationDisplayName();
#else
    return QString();
#endif
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

QString FeedbackConfigUiController::telemetryModeDescription(int telemetryIndex) const
{
    const auto name = d->appName();
    if (name.isEmpty()) {
        switch (telemetryIndexToMode(telemetryIndex)) {
            case Provider::NoTelemetry:
                return tr(
                    "We make this application for you. You can help us improve it by contributing information on how you use it. "
                    "This allows us to make sure we focus on things that matter to you.\n"
                    "Contributing statistics is of course entirely anonymous, will not use any kind of unique identifier and "
                    "will not cover any data you process with this application."
                );
            case Provider::BasicSystemInformation:
                return tr(
                    "Share basic system information. "
                    "No unique identification is included, nor data processed with the application."
                );
            case Provider::BasicUsageStatistics:
                return tr(
                    "Share basic system information and basic statistics on how often you use the application. "
                    "No unique identification is included, nor data processed with the application."
                );
            case Provider::DetailedSystemInformation:
                return tr(
                    "Share basic statistics on how often you use the application, as well as detailed information about your system. "
                    "No unique identification is included, nor data processed with the application."
                );
            case Provider::DetailedUsageStatistics:
                return tr(
                    "Share detailed system information and statistics on how often individual features of the application are used. "
                    "No unique identification is included, nor data processed with the application."
                );
        }
    } else {
        switch (telemetryIndexToMode(telemetryIndex)) {
            case Provider::NoTelemetry:
                return tr(
                    "We make %1 for you. You can help us improve it by contributing information on how you use it. "
                    "This allows us to make sure we focus on things that matter to you.\n"
                    "Contributing statistics is of course entirely anonymous, will not use any kind of unique identifier and "
                    "will not cover any data you process with %1."
                ).arg(name);
            case Provider::BasicSystemInformation:
                return tr(
                    "Share basic system information. "
                    "No unique identification is included, nor data processed with %1."
                ).arg(name);
            case Provider::BasicUsageStatistics:
                return tr(
                    "Share basic system information and basic statistics on how often you use %1. "
                    "No unique identification is included, nor data processed with %1."
                ).arg(name);
            case Provider::DetailedSystemInformation:
                return tr(
                    "Share basic statistics on how often you use %1, as well as detailed information about your system. "
                    "No unique identification is included, nor data processed with %1."
                ).arg(name);
            case Provider::DetailedUsageStatistics:
                return tr(
                    "Share detailed system information and statistics on how often individual features of %1 are used. "
                    "No unique identification is included, nor data processed with %1."
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
    const auto name = d->appName();
    if (name.isEmpty()) {
        switch (surveyIndex) {
            case 0:
                return tr(
                    "We make this application for you. In order to ensure it actually does what you need it to do we "
                    "would like to ask you about your use cases and your feedback, in the form of a web survey."
                );
            case 1:
                return tr(
                    "I will occasionally participate in web surveys about the application, not more than four times a year though."
                );
            case 2:
                return tr(
                    "I will participate in web surveys whenever one is available. Surveys can of course be deferred or skipped."
                );
        }
    } else {
        switch (surveyIndex) {
            case 0:
                return tr(
                    "We make %1 for you. In order to ensure it actually does what you need it to do we "
                    "would like to ask you about your use cases and your feedback, in the form of a web survey."
                ).arg(name);
            case 1:
                return tr(
                    "I will occasionally participate in web surveys about %1, not more than four times a year though."
                ).arg(name);
            case 2:
                return tr(
                    "I will participate in web surveys about %1 whenever one is available. Surveys can of course be deferred or skipped."
                ).arg(name);
        }
    }

    return QString();
}
