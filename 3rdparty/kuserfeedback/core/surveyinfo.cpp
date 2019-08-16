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
    s.setUuid(obj.value(QLatin1String("uuid")).toString());
    s.setUrl(QUrl(obj.value(QLatin1String("url")).toString()));
    s.setTarget(obj.value(QLatin1String("target")).toString());
    return s;
}
