/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "applicationversionsource.h"

#include <QCoreApplication>
#include <QVariant>

using namespace KUserFeedback;

ApplicationVersionSource::ApplicationVersionSource() :
    AbstractDataSource(QStringLiteral("applicationVersion"), Provider::BasicSystemInformation)
{
}

QString ApplicationVersionSource::description() const
{
    return tr("The version of the application.");
}

QVariant ApplicationVersionSource::data()
{
    if (QCoreApplication::applicationVersion().isEmpty())
        return QVariant();

    QVariantMap m;
    m.insert(QStringLiteral("value"), QCoreApplication::applicationVersion());
    return m;
}

QString ApplicationVersionSource::name() const
{
    return tr("Application version");
}
