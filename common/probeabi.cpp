/*
  probeabi.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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

#include "probeabi.h"

#include <QObject>
#include <QRegExp>
#include <QSharedData>
#include <QString>
#include <QStringList>

namespace GammaRay {

class ProbeABIPrivate : public QSharedData
{
  public:
    ProbeABIPrivate() :
      majorQtVersion(-1),
      minorQtVersion(-1),
      isDebug(false)
    {}

    ProbeABIPrivate(const ProbeABIPrivate &other) : QSharedData(other),
      architecture(other.architecture),
      compiler(other.compiler),
      majorQtVersion(other.majorQtVersion),
      minorQtVersion(other.minorQtVersion),
      isDebug(other.isDebug)
    {}

    QString architecture;
    QString compiler;
    int majorQtVersion;
    int minorQtVersion;
    bool isDebug;
};

}

using namespace GammaRay;

ProbeABI::ProbeABI() : d(new ProbeABIPrivate)
{
}

ProbeABI::ProbeABI(const ProbeABI& other) : d(other.d)
{
}

ProbeABI::~ProbeABI()
{
}

ProbeABI& ProbeABI::operator=(const ProbeABI& other)
{
  d = other.d;
  return *this;
}

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

void ProbeABI::setArchitecture(const QString& architecture)
{
  d->architecture = architecture;
}

QString ProbeABI::compiler() const
{
  return d->compiler;
}

void ProbeABI::setCompiler(const QString& compiler)
{
  d->compiler = compiler;
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
  return compiler() == "MSVC";
}

bool ProbeABI::isValid() const
{
  return hasQtVersion()
      && !d->architecture.isEmpty()
#ifdef Q_OS_WIN
      && !d->compiler.isEmpty()
#endif
      ;
}

bool ProbeABI::isCompatible(const ProbeABI& referenceABI) const
{
  return d->majorQtVersion == referenceABI.majorQtVersion()
      && d->minorQtVersion >= referenceABI.minorQtVersion() // we can work with older probes, since the target defines the Qt libraries being used
      && d->architecture == referenceABI.architecture()
#ifdef Q_OS_WIN
      && d->compiler == referenceABI.compiler()
#endif
      && (isDebugRelevant() ?  d->isDebug == referenceABI.isDebug() : true)
      ;
}

QString ProbeABI::id() const
{
  if (!isValid())
    return QString();

  QStringList idParts;
  idParts.push_back(QString("qt%1.%2").arg(majorQtVersion()).arg(minorQtVersion()));

#ifdef Q_OS_WIN
  idParts.push_back(compiler());
#endif

  if (isDebugRelevant())
    idParts.push_back(isDebug() ? "debug" : "release");

  idParts.push_back(architecture());

  return idParts.join("-");
}

ProbeABI ProbeABI::fromString(const QString& id)
{
  QStringList idParts = id.split('-');
  if (idParts.size() < 2)
    return ProbeABI();

  int index = 0;
  ProbeABI abi;

  // version
  static QRegExp versionRegExp("^qt(\\d+)\\.(\\d+)$");
  if (versionRegExp.indexIn(idParts.value(index++)) != 0)
    return ProbeABI();
  abi.setQtVersion(versionRegExp.cap(1).toInt(), versionRegExp.cap(2).toInt());

  // compiler
#ifdef Q_OS_WIN
  abi.setCompiler(idParts.value(index++));
#endif

  // debug/release
  if (abi.isDebugRelevant()) {
    if (idParts.size() <= index)
      return ProbeABI();
    const QString s = idParts.value(index++);
    if (s != "release" && s != "debug")
      return ProbeABI();
    abi.setIsDebug(s == "debug");
  }

  // architecture
  if (idParts.size() != index + 1)
    return ProbeABI();
  abi.setArchitecture(idParts.value(index));
  return abi;
}

QString ProbeABI::displayString() const
{
  if (!isValid())
    return QString();

  QStringList details;
#ifdef Q_OS_WIN
  details.push_back(compiler());
#endif
  if (isDebugRelevant())
    details.push_back(isDebug() ? QObject::tr("debug") : QObject::tr("release"));
  details.push_back(architecture());

  return QObject::tr("Qt %1.%2 (%3)")
    .arg(majorQtVersion())
    .arg(minorQtVersion())
    .arg(details.join(", "));
}

bool ProbeABI::operator==(const ProbeABI& rhs) const
{
  return majorQtVersion() == rhs.majorQtVersion()
    && minorQtVersion() == rhs.minorQtVersion()
    && architecture() == rhs.architecture()
    && compiler() == rhs.compiler()
    && isDebug() == rhs.isDebug();
}

bool ProbeABI::operator<(const ProbeABI& rhs) const
{
  if (majorQtVersion() == rhs.majorQtVersion())
    return minorQtVersion() < rhs.minorQtVersion();
  return majorQtVersion() < rhs.majorQtVersion();
}
