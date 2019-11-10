/*
  probeabi.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <config-gammaray.h>
#include "probeabi.h"

#include <QCoreApplication>
#include <QObject>
#include <QRegExp>
#include <QSharedData>
#include <QString>
#include <QStringList>

namespace GammaRay {
class ProbeABIContext
{
    Q_DECLARE_TR_FUNCTIONS(GammaRay::ProbeABIContext)
};


class ProbeABIPrivate : public QSharedData
{
public:
    ProbeABIPrivate() = default;

    ProbeABIPrivate(const ProbeABIPrivate &) = default;

    QString architecture;
    QString compiler;
    QString compilerVersion;
    int majorQtVersion = -1;
    int minorQtVersion = -1;
    bool isDebug = false;
};
}

using namespace GammaRay;

ProbeABI::ProbeABI()
    : d(new ProbeABIPrivate)
{
}

ProbeABI::ProbeABI(const ProbeABI &) = default;

ProbeABI::~ProbeABI() = default;

ProbeABI &ProbeABI::operator=(const ProbeABI &) = default;

int ProbeABI::majorQtVersion() const
{
    return d->majorQtVersion;
}

int ProbeABI::minorQtVersion() const
{
    return d->minorQtVersion;
}

void ProbeABI::setQtVersion(int major, int minor)
{
    d->majorQtVersion = major;
    d->minorQtVersion = minor;
}

bool ProbeABI::hasQtVersion() const
{
    return d->majorQtVersion > 0 && d->minorQtVersion >= 0;
}

QString ProbeABI::architecture() const
{
    return d->architecture;
}

void ProbeABI::setArchitecture(const QString &architecture)
{
    d->architecture = architecture;
}

QString ProbeABI::compiler() const
{
    return d->compiler;
}

void ProbeABI::setCompiler(const QString &compiler)
{
    d->compiler = compiler;
}

QString ProbeABI::compilerVersion() const
{
    return d->compilerVersion;
}

void ProbeABI::setCompilerVersion(const QString &compilerVersion)
{
    d->compilerVersion = compilerVersion;
}

bool ProbeABI::isVersionRelevant() const
{
    return compiler() == QLatin1String("MSVC");
}

bool ProbeABI::isDebug() const
{
    return d->isDebug;
}

void ProbeABI::setIsDebug(bool debug)
{
    d->isDebug = debug;
}

bool ProbeABI::isDebugRelevant() const
{
#if defined(Q_OS_MAC) || defined(Q_OS_WIN)
    return true;
#else
    return false;
#endif
}

bool ProbeABI::isValid() const
{
    return hasQtVersion()
           && !d->architecture.isEmpty()
#ifdef Q_OS_WIN
           && !d->compiler.isEmpty()
           && (!isVersionRelevant() || !d->compilerVersion.isEmpty())
#endif
    ;
}

bool ProbeABI::isCompatible(const ProbeABI &referenceABI) const
{
    return d->majorQtVersion == referenceABI.majorQtVersion()
           && d->minorQtVersion >= referenceABI.minorQtVersion() // we can work with older probes, since the target defines the Qt libraries being used
           && d->architecture == referenceABI.architecture()
#ifdef Q_OS_WIN
           && d->compiler == referenceABI.compiler()
           && d->compilerVersion == referenceABI.compilerVersion()
#endif
           && (isDebugRelevant() ? d->isDebug == referenceABI.isDebug() : true)
    ;
}

QString ProbeABI::id() const
{
    if (!isValid())
        return QString();

    QStringList idParts;
    idParts.push_back(QStringLiteral("qt%1_%2").arg(majorQtVersion()).arg(minorQtVersion()));

#ifdef Q_OS_WIN
    idParts.push_back(compiler());
    if (isVersionRelevant())
        idParts.push_back(compilerVersion());
#endif

    idParts.push_back(architecture());
    return idParts.join(QStringLiteral("-")).append(isDebugRelevant()
                                                    && isDebug() ? QStringLiteral(
                                                        GAMMARAY_DEBUG_POSTFIX) : QString());
}

ProbeABI ProbeABI::fromString(const QString &id)
{
    QStringList idParts = id.split('-');
    if (idParts.size() < 2)
        return ProbeABI();

    int index = 0;
    ProbeABI abi;

    // version
    static QRegExp versionRegExp(R"(^qt(\d+)\_(\d+)$)");
    if (versionRegExp.indexIn(idParts.value(index++)) != 0)
        return ProbeABI();
    abi.setQtVersion(versionRegExp.cap(1).toInt(), versionRegExp.cap(2).toInt());

    // compiler
#ifdef Q_OS_WIN
    abi.setCompiler(idParts.value(index++));
    if (abi.isVersionRelevant())
        abi.setCompilerVersion(idParts.value(index++));
#endif

    if (idParts.size() != index + 1)
        return ProbeABI();

    // architecture / debug/release
    const QString postfix = QStringLiteral(GAMMARAY_DEBUG_POSTFIX);
    QString arch = idParts.value(index);

    if (!postfix.isEmpty()) {
        if (arch.endsWith(postfix, Qt::CaseInsensitive)) {
            arch.chop(postfix.length());

            if (abi.isDebugRelevant())
                abi.setIsDebug(true);
        }
    }

    abi.setArchitecture(arch);
    return abi;
}

QString ProbeABI::displayString() const
{
    if (!isValid())
        return QString();

    QStringList details;
#ifdef Q_OS_WIN
    details.push_back(compiler());
    if (isVersionRelevant())
        details.push_back(compilerVersion());
#endif
    if (isDebugRelevant())
        details.push_back(isDebug() ? ProbeABIContext::tr("debug") : ProbeABIContext::tr("release"));

    details.push_back(architecture());

    return ProbeABIContext::tr("Qt %1.%2 (%3)")
           .arg(majorQtVersion())
           .arg(minorQtVersion())
           .arg(details.join(QStringLiteral(", ")));
}

bool ProbeABI::operator==(const ProbeABI &rhs) const
{
    return majorQtVersion() == rhs.majorQtVersion()
           && minorQtVersion() == rhs.minorQtVersion()
           && architecture() == rhs.architecture()
           && compiler() == rhs.compiler()
           && compilerVersion() == rhs.compilerVersion()
           && isDebug() == rhs.isDebug();
}

bool ProbeABI::operator<(const ProbeABI &rhs) const
{
    if (majorQtVersion() == rhs.majorQtVersion())
        return minorQtVersion() < rhs.minorQtVersion();
    return majorQtVersion() < rhs.majorQtVersion();
}
