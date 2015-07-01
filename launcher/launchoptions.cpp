/*
  launchoptions.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2013-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "launchoptions.h"

#include <common/probeabi.h>

#include <QVariant>
#include <QProcess>
#include <QFileInfo>
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QStandardPaths>
#endif

namespace GammaRay {

class LaunchOptionsPrivate {
public:
    LaunchOptionsPrivate()
        : m_pid(-1)
        , m_uiMode(LaunchOptions::OutOfProcessUi)
    {}

public:
    QString m_probePath;
    QStringList m_launchArguments;
    QString m_injectorType;
    ProbeABI m_probeABI;
    int m_pid;
    LaunchOptions::UiMode m_uiMode;
    QHash<QByteArray, QByteArray> m_probeSettings;
};

LaunchOptions::LaunchOptions() :
  p(new LaunchOptionsPrivate)
{
}

LaunchOptions::~LaunchOptions()
{
    delete p;
}

bool LaunchOptions::isLaunch() const
{
  return !p->m_launchArguments.isEmpty();
}

bool LaunchOptions::isAttach() const
{
  return pid() > 0;
}

bool LaunchOptions::isValid() const
{
  return isLaunch() != isAttach();
}

void LaunchOptions::setProbePath(const QString &path)
{
  p->m_probePath = path;
}

const QString &LaunchOptions::probePath() const
{
  return p->m_probePath;
}

QStringList LaunchOptions::launchArguments() const
{
  return p->m_launchArguments;
}

void LaunchOptions::setLaunchArguments(const QStringList& args)
{
  p->m_launchArguments = args;
  Q_ASSERT(p->m_pid <= 0 || p->m_launchArguments.isEmpty());
}

QString LaunchOptions::absoluteExecutablePath() const
{
  if (p->m_launchArguments.isEmpty())
    return QString();

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
  QString path = p->m_launchArguments.first();
  const QFileInfo fi(path);
  if (fi.isFile() && fi.isExecutable())
    return path;
  path = QStandardPaths::findExecutable(p->m_launchArguments.first());
  if (!path.isEmpty())
    return path;
#endif

  return p->m_launchArguments.first();
}

int LaunchOptions::pid() const
{
  return p->m_pid;
}

void LaunchOptions::setPid(int pid)
{
  p->m_pid = pid;
  Q_ASSERT(p->m_pid <= 0 || p->m_launchArguments.isEmpty());
}

LaunchOptions::UiMode LaunchOptions::uiMode() const
{
  return p->m_uiMode;
}

void LaunchOptions::setUiMode(LaunchOptions::UiMode mode)
{
  p->m_uiMode = mode;
  setProbeSetting("InProcessUi", mode == InProcessUi);
}

QString LaunchOptions::injectorType() const
{
  return p->m_injectorType;
}

void LaunchOptions::setInjectorType(const QString& injectorType)
{
  p->m_injectorType = injectorType;
}

const ProbeABI &LaunchOptions::probeABI() const
{
  return p->m_probeABI;
}

void LaunchOptions::setProbeABI(const ProbeABI& abi)
{
  p->m_probeABI = abi;
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

  p->m_probeSettings.insert(key.toUtf8(), v);
}

QHash< QByteArray, QByteArray > LaunchOptions::probeSettings() const
{
  return p->m_probeSettings;
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

  if (p->m_probeABI.isValid()) {
    args.push_back("--probe");
    args.push_back(p->m_probeABI.id());
  }

  if (p->m_probeSettings.contains("ServerAddress")) {
    args.push_back("--listen");
    args.push_back(p->m_probeSettings.value("ServerAddress"));
  }
  if (p->m_probeSettings.value("RemoteAccessEnabled") == "false")
    args.push_back("--no-listen");

  if (isAttach()) {
    args.push_back("--pid");
    args.push_back(QString::number(pid()));
  } else {
    args += launchArguments();
  }
  return QProcess::startDetached(launcherPath, args);
}

LaunchOptions::LaunchOptions(const LaunchOptions &other)
    : p(new LaunchOptionsPrivate)
{
    *p = *other.p;
}

LaunchOptions &LaunchOptions::operator=(const LaunchOptions &other)
{
    *p = *other.p;
    return *this;
}

}
