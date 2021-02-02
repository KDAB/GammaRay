/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "usagetimesource.h"
#include "abstractdatasource_p.h"
#include "provider_p.h"

#include <QVariant>

using namespace KUserFeedback;

namespace KUserFeedback {
class UsageTimeSourcePrivate : public AbstractDataSourcePrivate
{
public:
    UsageTimeSourcePrivate() : provider(nullptr) {}
    ProviderPrivate *provider;
};
}

UsageTimeSource::UsageTimeSource() :
    AbstractDataSource(QStringLiteral("usageTime"), Provider::BasicUsageStatistics, new UsageTimeSourcePrivate)
{
}

QString UsageTimeSource::description() const
{
    return tr("The total amount of time the application has been used.");
}

QVariant UsageTimeSource::data()
{
    Q_D(UsageTimeSource);
    Q_ASSERT(d->provider);

    QVariantMap m;
    m.insert(QStringLiteral("value"), d->provider->currentApplicationTime());
    return m;
}

QString UsageTimeSource::name() const
{
    return tr("Usage time");
}

void UsageTimeSource::setProvider(ProviderPrivate* p)
{
    Q_D(UsageTimeSource);
    d->provider = p;
}
