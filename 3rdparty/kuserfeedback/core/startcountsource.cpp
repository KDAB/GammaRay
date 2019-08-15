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

#include "startcountsource.h"
#include "abstractdatasource_p.h"
#include "provider_p.h"

#include <QVariant>

using namespace KUserFeedback;

namespace KUserFeedback {
class StartCountSourcePrivate : public AbstractDataSourcePrivate
{
public:
    StartCountSourcePrivate() : provider(nullptr) {}
    ProviderPrivate *provider;
};
}

StartCountSource::StartCountSource() :
    AbstractDataSource(QStringLiteral("startCount"), Provider::BasicUsageStatistics, new StartCountSourcePrivate)
{
}

QString StartCountSource::description() const
{
    return tr("How often the application has been started.");
}

QVariant StartCountSource::data()
{
    Q_D(StartCountSource);
    Q_ASSERT(d->provider);

    QVariantMap m;
    m.insert(QStringLiteral("value"), d->provider->startCount);
    return m;
}

QString StartCountSource::name() const
{
    return tr("Launches count");
}

void StartCountSource::setProvider(ProviderPrivate *p)
{
    Q_D(StartCountSource);
    d->provider = p;
}
