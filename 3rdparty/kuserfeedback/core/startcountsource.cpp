/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
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
