/*
  probeabi.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#include <config-gammaray.h>
#include "probeabi.h"

#include <QCoreApplication>
#include <QObject>
#include <QRegularExpression>
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

bool ProbeABI::isDebugRelevant()
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
        && (isDebugRelevant() ? d->isDebug == referenceABI.isDebug() : true);
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
    return idParts.join(QStringLiteral("-")).append(isDebugRelevant() && isDebug() ? QStringLiteral(GAMMARAY_DEBUG_POSTFIX) : QString());
}

ProbeABI ProbeABI::fromString(const QString &id)
{
    QStringList idParts = id.split('-');
    if (idParts.size() < 2)
        return ProbeABI();

    int index = 0;
    ProbeABI abi;

    // version
    static const QRegularExpression versionRegExp(R"(^qt(\d+)\_(\d+)$)");
    const auto match = versionRegExp.match(idParts.value(index++));
    if (!match.hasMatch())
        return ProbeABI();
    abi.setQtVersion(match.captured(1).toInt(), match.captured(2).toInt());

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
        return ProbeABIContext::tr("Unknown ABI");

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
