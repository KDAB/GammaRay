#include "clientlauncher.h"

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
  m_process.start("gammaray-client", QStringList() << hostName);
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
