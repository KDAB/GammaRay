/*
    SPDX-FileCopyrightText: 2016 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "platforminfosource.h"

#include <QSysInfo>
#include <QVariant>

using namespace KUserFeedback;

PlatformInfoSource::PlatformInfoSource() :
    AbstractDataSource(QStringLiteral("platform"), Provider::BasicSystemInformation)
{
}

QString PlatformInfoSource::description() const
{
    return tr("Type and version of the operating system.");
}

QVariant PlatformInfoSource::data()
{
    QVariantMap m;
#if (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
    // on Linux productType() is the distro name
    m.insert(QStringLiteral("os"), QStringLiteral("linux"));

    // openSUSE Tumbleweed has the current date as version number, that is a bit too precise for us
    if (QSysInfo::productType() == QLatin1String("opensuse") && QSysInfo::productVersion().startsWith(QLatin1String("201"))) {
        // old form in use until early 2018
        m.insert(QStringLiteral("version"), QString(QSysInfo::productType() + QLatin1String("-tumbleweed")));
    } else if (QSysInfo::productType() == QLatin1String("opensuse-tumbleweed")) {
        // new form in use since early 2018
        m.insert(QStringLiteral("version"), QSysInfo::productType());
    }
    // Arch and other rolling-release distros set productVersion to "unknown"
    else if (QSysInfo::productVersion() == QLatin1String("unknown")) {
        m.insert(QStringLiteral("version"), QSysInfo::productType());
    } else {
        m.insert(QStringLiteral("version"), QString(QSysInfo::productType() + QLatin1Char('-') + QSysInfo::productVersion()));
    }
#elif defined(Q_OS_MAC)
    // QSysInfo::productType() on macOS alternates between "osx" and "macos"...
    m.insert(QStringLiteral("os"), QStringLiteral("macos"));
    m.insert(QStringLiteral("version"), QSysInfo::productVersion());
#else
    m.insert(QStringLiteral("os"), QSysInfo::productType());
    m.insert(QStringLiteral("version"), QSysInfo::productVersion());
#endif
    return m;
}

QString PlatformInfoSource::name() const
{
    return tr("Platform information");
}
