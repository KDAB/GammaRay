#include "injectorstyleplugin.h"

#include <QDebug>
#include <QStyleFactory>

#include <3rdparty/qt/qguiplatformplugin_p.h>

#include <dlfcn.h>

using namespace Endoscope;

QStyle* InjectorStylePlugin::create(const QString& )
{
  qDebug() << Q_FUNC_INFO;
  inject();
  static QGuiPlatformPlugin defaultGuiPlatform;
  return QStyleFactory::create( defaultGuiPlatform.styleName() );
}

QStringList InjectorStylePlugin::keys() const
{
  return QStringList() << QLatin1String("endoscope-injector");
}

void InjectorStylePlugin::inject()
{
  void* probeDllHandle = dlopen( "endoscope_probe.so", RTLD_NOW );
  if ( !probeDllHandle ) {
    qWarning() << dlerror();
    return;
  }

  void* probeFuncHandle = dlsym( probeDllHandle, "endoscope_probe_inject" );
  qDebug() << probeFuncHandle;
  if ( probeFuncHandle )
    reinterpret_cast<void(*)()>( probeFuncHandle )();
  else
    qWarning() << dlerror();
}

Q_EXPORT_PLUGIN2(endoscope_injector_style, Endoscope::InjectorStylePlugin)

#include "injectorstyleplugin.moc"
