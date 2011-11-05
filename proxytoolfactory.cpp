#include "proxytoolfactory.h"

#include <qpluginloader.h>
#include <qsettings.h>
#include <qfileinfo.h>
#include <qdir.h>
#include <QtGui/QLabel>

#include <iostream>

using namespace GammaRay;

ProxyToolFactory::ProxyToolFactory(const QString& path, QObject* parent) :
  QObject( parent ),
  m_factory( 0 )
{
  const QFileInfo pluginInfo(path);
  m_id = pluginInfo.baseName();
  
  QSettings desktopFile(path, QSettings::IniFormat);
  desktopFile.beginGroup( QLatin1String("Desktop Entry") );
  m_name = desktopFile.value(QLatin1String("Name")).toString();
  m_supportedTypes = desktopFile.value(QLatin1String("X-GammaRay-Types")).toString().split(QLatin1Char(';'), QString::SkipEmptyParts);
  m_pluginPath = pluginInfo.dir().absoluteFilePath( desktopFile.value(QLatin1String("Exec")).toString() );

  const QString dllBaseName = desktopFile.value(QLatin1String("Exec")).toString();
  if (dllBaseName.isEmpty())
    return;

  foreach( const QString &entry, pluginInfo.dir().entryList( QStringList(dllBaseName + QLatin1Char('*')), QDir::Files) ) {
    const QString path = pluginInfo.dir().absoluteFilePath(entry);
    if (QLibrary::isLibrary(path)) {
      m_pluginPath = path;
      break;;
    }
  }
}

bool ProxyToolFactory::isValid() const
{
  return !m_id.isEmpty() && !m_name.isEmpty() && !m_pluginPath.isEmpty() && !m_supportedTypes.isEmpty();
}

QString ProxyToolFactory::id() const
{
  return m_id;
}

QString ProxyToolFactory::name() const
{
  return m_name;
}

QStringList ProxyToolFactory::supportedTypes() const
{
  return m_supportedTypes;
}

void ProxyToolFactory::init(ProbeInterface* probe)
{
  QPluginLoader loader( m_pluginPath, this );
  m_factory = qobject_cast<ToolFactory*>(loader.instance());
  if (!m_factory) {
    std::cerr << "error loading plugin " << qPrintable(m_pluginPath) << ": " << qPrintable(loader.errorString()) << std::endl;
    return;
  }
  Q_ASSERT(m_factory);
  m_factory->init( probe );
}

QWidget* ProxyToolFactory::createWidget(ProbeInterface* probe, QWidget* parentWidget)
{
  if (!m_factory)
    return new QLabel( tr("Plugin '%1' could not be loaded.").arg(m_pluginPath), parentWidget);
  Q_ASSERT(m_factory);
  return m_factory->createWidget(probe, parentWidget);
}

#include "proxytoolfactory.moc"
