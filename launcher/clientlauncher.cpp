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

bool ClientLauncher::launch(const QString& hostName)
{
  const QString clientPath = QCoreApplication::applicationDirPath() + QDir::separator() + QLatin1String("gammaray-client");
  if (QFile::exists(clientPath)) {
    m_process.start(clientPath, QStringList() << hostName);
  } else {
    qWarning() << "gammaray-client executable not found in the expected location (" << QCoreApplication::applicationDirPath() << "), "
               << "continuing anyway, hoping for it to be in PATH.";
    m_process.start("gammaray-client", QStringList() << hostName);
  }
  return m_process.waitForStarted();
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
