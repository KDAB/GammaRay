#include "launchoptions.h"

#include <QVariant>

using namespace GammaRay;

LaunchOptions::LaunchOptions() :
  m_pid(-1)
{
}

LaunchOptions::~LaunchOptions()
{
}

bool LaunchOptions::isLaunch() const
{
  return !m_launchArguments.isEmpty();
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
  return m_launchArguments;
}

void LaunchOptions::setLaunchArguments(const QStringList& args)
{
  m_launchArguments = args;
  Q_ASSERT(m_pid <= 0 || m_launchArguments.isEmpty());
}

int LaunchOptions::pid() const
{
  return m_pid;
}

void LaunchOptions::setPid(int pid)
{
  m_pid = pid;
  Q_ASSERT(m_pid <= 0 || m_launchArguments.isEmpty());
}

void LaunchOptions::setUseInProcessUi(bool enable)
{
  setProbeSetting("InProcessUi", enable);
}

void LaunchOptions::sendProbeSettings()
{
  // for now just use env vars, in order to make this work with attaching as well
  // we are going to need temporary files, shared memory, or something like that
  // cf. GammaRay::ProbeSettings for the receiving side of this
  for (QHash<QByteArray, QByteArray>::const_iterator it = m_probeSettings.constBegin(); it != m_probeSettings.constEnd(); ++it)
    qputenv("GAMMARAY_" + it.key(), it.value());
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

  m_probeSettings.insert(key.toUtf8(), v);
}
