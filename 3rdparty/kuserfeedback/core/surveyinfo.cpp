/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "surveyinfo.h"

#include <QJsonObject>
#include <QSharedData>
#include <QUrl>
#include <QUuid>

using namespace KUserFeedback;

class KUserFeedback::SurveyInfoData : public QSharedData
{
public:
    QUuid uuid;
    QUrl url;
    QString target;
};


SurveyInfo::SurveyInfo() : d (new SurveyInfoData)
{
}

SurveyInfo::SurveyInfo(const SurveyInfo &other) :
    d(other.d)
{
}

SurveyInfo::~SurveyInfo()
{
}

SurveyInfo& SurveyInfo::operator=(const SurveyInfo &other)
{
    d = other.d;
    return *this;
}

bool SurveyInfo::isValid() const
{
    return !d->uuid.isNull() && d->url.isValid();
}

QUuid SurveyInfo::uuid() const
{
    return d->uuid;
}

void SurveyInfo::setUuid(const QUuid &id)
{
    d->uuid = id;
}

QUrl SurveyInfo::url() const
{
    return d->url;
}

void SurveyInfo::setUrl(const QUrl& url)
{
    d->url = url;
}

QString SurveyInfo::target() const
{
    return d->target;
}

void SurveyInfo::setTarget(const QString &target)
{
    d->target = target;
}

SurveyInfo SurveyInfo::fromJson(const QJsonObject& obj)
{
    SurveyInfo s;
    s.setUuid(QUuid(obj.value(QLatin1String("uuid")).toString()));
    s.setUrl(QUrl(obj.value(QLatin1String("url")).toString()));
    s.setTarget(obj.value(QLatin1String("target")).toString());
    return s;
}
