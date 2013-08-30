#include "clientlauncher.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>

using namespace GammaRay;

ClientLauncher::ClientLauncher()
{
  m_process.setProcessChannelMode(QProcess::ForwardedChannels);
}

ClientLauncher::~ClientLauncher()
{
}

QString ClientLauncher::clientPath()
{
  const QString path = QCoreApplication::applicationDirPath() + QDir::separator() + QLatin1String("gammaray-client");
  if (QFile::exists(path))
    return path;
  qWarning() << "gammaray-client executable not found in the expected location (" << QCoreApplication::applicationDirPath() << "), "
             << "continuing anyway, hoping for it to be in PATH.";
  return "gammaray-client";
}

QStringList ClientLauncher::makeArgs(const QString& hostName, quint16 port)
{
  QStringList args;
  args.push_back(hostName);
  if (port > 0)
    args.push_back(QString::number(port));
  return args;
}

bool ClientLauncher::launch(const QString& hostName, quint16 port)
{
  m_process.start(clientPath(), makeArgs(hostName, port));
  return m_process.waitForStarted();
}

void ClientLauncher::launchDetached(const QString& hostName, quint16 port)
{
  QProcess::startDetached(clientPath(), makeArgs(hostName, port));
}

void ClientLauncher::terminate()
{
  if (m_process.state() == QProcess::Running)
    m_process.terminate();
}

void ClientLauncher::waitForFinished()
{
  if (m_process.state() == QProcess::Running)
    m_process.waitForFinished(-1);
}
