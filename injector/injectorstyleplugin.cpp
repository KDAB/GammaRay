/*
  injectorstyleplugin.cpp

  This file is part of Endoscope, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krauss@kdab.com>

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
  const QByteArray probeDll = qgetenv( "ENDOSCOPE_STYLEINJECTOR_PROBEDLL" );
  if ( probeDll.isEmpty() ) {
    qWarning("No probe DLL specified.");
    return;
  }

  void* probeDllHandle = dlopen( probeDll, RTLD_NOW );
  if ( !probeDllHandle ) {
    qWarning() << dlerror();
    return;
  }

  const QByteArray probeFunc = qgetenv( "ENDOSCOPE_STYLEINJECTOR_PROBEFUNC" );
  if ( probeFunc.isEmpty() )
    return;
  void* probeFuncHandle = dlsym( probeDllHandle, probeFunc );
  if ( probeFuncHandle )
    reinterpret_cast<void(*)()>( probeFuncHandle )();
  else
    qWarning() << dlerror();
}

Q_EXPORT_PLUGIN2(endoscope_injector_style, Endoscope::InjectorStylePlugin)

#include "injectorstyleplugin.moc"
