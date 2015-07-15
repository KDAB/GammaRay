/*
  launchoptions.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "launchoptions.h"
#include "probeabi.h"

#include <QVariant>
#include <QProcess>
#include <QFileInfo>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QStandardPaths>
#endif

namespace GammaRay {

class LaunchOptionsPrivate : public QSharedData
{
public:
    LaunchOptionsPrivate() :
        pid(-1),
        uiMode(LaunchOptions::OutOfProcessUi)
    {}

    QStringList launchArguments;
    QString injectorType;
    ProbeABI probeABI;
    int pid;
    LaunchOptions::UiMode uiMode;
    QHash<QByteArray, QByteArray> probeSettings;
};

}

using namespace GammaRay;

LaunchOptions::LaunchOptions() :
    d(new LaunchOptionsPrivate)
{
}

LaunchOptions::~LaunchOptions()
{
}

LaunchOptions::LaunchOptions(const LaunchOptions& other) :
    d(other.d)
{
}

LaunchOptions& LaunchOptions::operator=(const LaunchOptions& other)
{
    d = other.d;
    return *this;
}

bool LaunchOptions::isLaunch() const
{
  return !d->launchArguments.isEmpty();
}

bool LaunchOptions::isAttach() const
{
  return pid() > 0;
}

bool LaunchOptions::isValid() const
{
  return isLaunch() != isAttach();
}

QStringList LaunchOptions::launchArguments() const
{
  return d->launchArguments;
}

void LaunchOptions::setLaunchArguments(const QStringList& args)
{
  d->launchArguments = args;
  Q_ASSERT(d->pid <= 0 || d->launchArguments.isEmpty());
}

QString LaunchOptions::absoluteExecutablePath() const
{
  if (d->launchArguments.isEmpty())
    return QString();

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  QString path = d->launchArguments.first();
  const QFileInfo fi(path);
  if (fi.isFile() && fi.isExecutable())
    return path;
  path = QStandardPaths::findExecutable(d->launchArguments.first());
  if (!path.isEmpty())
    return path;
#endif

  return d->launchArguments.first();
}

int LaunchOptions::pid() const
{
  return d->pid;
}

void LaunchOptions::setPid(int pid)
{
  d->pid = pid;
  Q_ASSERT(d->pid <= 0 || d->launchArguments.isEmpty());
}

LaunchOptions::UiMode LaunchOptions::uiMode() const
{
  return d->uiMode;
}

void LaunchOptions::setUiMode(LaunchOptions::UiMode mode)
{
  d->uiMode = mode;
  setProbeSetting("InProcessUi", mode == InProcessUi);
}

QString LaunchOptions::injectorType() const
{
  return d->injectorType;
}

void LaunchOptions::setInjectorType(const QString& injectorType)
{
  d->injectorType = injectorType;
}

ProbeABI LaunchOptions::probeABI() const
{
  return d->probeABI;
}

void LaunchOptions::setProbeABI(const ProbeABI& abi)
{
  d->probeABI = abi;
}

QString LaunchOptions::probePath() const
{
  return d->probeSettings.value("ProbePath");
}

void LaunchOptions::setProbePath(const QString& path)
{
  setProbeSetting("ProbePath", path);
}

void LaunchOptions::setProbeSetting(const QString& key, const QVariant& value)
{
  QByteArray v;
  switch (value.type()) {
    case QVariant::String:
      v = value.toString().toUtf8();
      break;
    case QVariant::Bool:
      v = value.toBool() ? "true" : "false";
      break;
    case QVariant::Int:
      v = QByteArray::number(value.toInt());
      break;
    default:
      qFatal("unsupported probe settings type");
  }

  d->probeSettings.insert(key.toUtf8(), v);
}

QHash< QByteArray, QByteArray > LaunchOptions::probeSettings() const
{
  return d->probeSettings;
}

bool LaunchOptions::execute(const QString& launcherPath) const
{
  Q_ASSERT(!launcherPath.isEmpty());
  Q_ASSERT(isValid());

  QStringList args;
  switch (uiMode()) {
    case InProcessUi:
      args.push_back("--inprocess");
      break;
    case OutOfProcessUi:
      args.push_back("--no-inprocess");
      break;
    case NoUi:
      args.push_back("--inject-only");
      break;
  }

  if (d->probeABI.isValid()) {
    args.push_back("--probe");
    args.push_back(d->probeABI.id());
  }

  if (d->probeSettings.contains("ServerAddress")) {
    args.push_back("--listen");
    args.push_back(d->probeSettings.value("ServerAddress"));
  }
  if (d->probeSettings.value("RemoteAccessEnabled") == "false")
    args.push_back("--no-listen");

  if (isAttach()) {
    args.push_back("--pid");
    args.push_back(QString::number(pid()));
  } else {
    args += launchArguments();
  }
  return QProcess::startDetached(launcherPath, args);
}
