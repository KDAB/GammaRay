/*
  probeabi.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

bool ProbeABI::isValid() const
{
  return d->majorQtVersion >= 0
      && d->minorQtVersion >= 0
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
      && d->isDebug == referenceABI.isDebug()
#endif
      ;
}

QString ProbeABI::id() const
{
  if (!isValid())
    return QString();

#ifndef Q_OS_WIN
  return QString("qt%1.%2-%3")
    .arg(majorQtVersion())
    .arg(minorQtVersion())
    .arg(architecture());
#else
  return QString("qt%1.%2-%3-%4-%5")
    .arg(majorQtVersion())
    .arg(minorQtVersion())
    .arg(compiler())
    .arg(isDebug() ? "debug" : "release")
    .arg(architecture());
#endif
}

ProbeABI ProbeABI::fromString(const QString& id)
{
#ifndef Q_OS_WIN
  static QRegExp regExp("^qt(\\d+)\\.(\\d+)-(.+)$");
#else
  static QRegExp regExp("^qt(\\d+)\\.(\\d+)-([^-]+)-(debug|release)-(.+)$");
#endif

  if (regExp.indexIn(id) != 0)
    return ProbeABI();

  ProbeABI abi;
  abi.setQtVersion(regExp.cap(1).toInt(), regExp.cap(2).toInt());
#ifndef Q_OS_WIN
  abi.setArchitecture(regExp.cap(3));
#else
  abi.setCompiler(regExp.cap(3));
  abi.setIsDebug(regExp.cap(4) == "debug");
  abi.setArchitecture(regExp.cap(5));
#endif

  return abi;
}

QString ProbeABI::displayString() const
{
  if (!isValid())
    return QString();

#ifndef Q_OS_WIN
  return QObject::tr("Qt %1.%2 (%3)")
    .arg(majorQtVersion())
    .arg(minorQtVersion())
    .arg(architecture());
#else
    return QObject::tr("Qt %1.%2 (%3, %4, %5)")
      .arg(majorQtVersion())
      .arg(minorQtVersion())
      .arg(compiler())
      .arg(isDebug() ? QObject::tr("debug") : QObject::tr("release"))
      .arg(architecture());
#endif
}

bool ProbeABI::operator<(const ProbeABI& rhs) const
{
  if (majorQtVersion() == rhs.majorQtVersion())
    return minorQtVersion() < rhs.minorQtVersion();
  return majorQtVersion() < rhs.majorQtVersion();
}
