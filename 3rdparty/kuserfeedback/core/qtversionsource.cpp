/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "qtversionsource.h"

#include <QVariant>

using namespace KUserFeedback;

QtVersionSource::QtVersionSource() :
    AbstractDataSource(QStringLiteral("qtVersion"), Provider::BasicSystemInformation)
{
}

QString QtVersionSource::description() const
{
    return tr("The Qt version used by this application.");
}

QVariant QtVersionSource::data()
{
    QVariantMap m;
    m.insert(QStringLiteral("value"), QString::fromLatin1(qVersion()));
    return m;
}

QString QtVersionSource::name() const
{
    return tr("Qt version information");
}
