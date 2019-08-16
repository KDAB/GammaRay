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
