/*
    SPDX-FileCopyrightText: 2017 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: MIT
*/

#include "compilerinfosource.h"

#include <QVariant>

using namespace KUserFeedback;

CompilerInfoSource::CompilerInfoSource()
    : AbstractDataSource(QStringLiteral("compiler"), Provider::BasicSystemInformation)
{
}

QString CompilerInfoSource::description() const
{
    return tr("The compiler used to build this application.");
}

#define STRINGIFY(x) #x
#define INT2STR(x) STRINGIFY(x)

QVariant CompilerInfoSource::data()
{
    QVariantMap m;

#ifdef Q_CC_GNU
    m.insert(QStringLiteral("type"), QStringLiteral("GCC"));
    m.insert(QStringLiteral("version"), QString::fromLatin1( "" INT2STR(__GNUC__) "." INT2STR(__GNUC_MINOR__)));
#endif

#ifdef Q_CC_CLANG
    m.insert(QStringLiteral("type"), QStringLiteral("Clang"));
    m.insert(QStringLiteral("version"), QString::fromLatin1( "" INT2STR(__clang_major__) "." INT2STR(__clang_minor__)));
#endif

#ifdef Q_CC_MSVC
    m.insert(QStringLiteral("type"), QStringLiteral("MSVC"));
    m.insert(QStringLiteral("version"), QString::fromLatin1( "" INT2STR(_MSC_VER)));
#endif

    if (m.isEmpty())
        m.insert(QStringLiteral("type"), QStringLiteral("unknown"));

    return m;
}

QString CompilerInfoSource::name() const
{
    return tr("Compiler information");
}

#undef STRINGIFY
#undef INT2STR
