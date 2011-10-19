/*
  injectorstyleplugin.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2011 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "injectorstyleplugin.h"

#include <QDebug>
#include <QStyleFactory>

#include <3rdparty/qt/qguiplatformplugin_p.h>

#ifdef _WIN32
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

using namespace GammaRay;

QStyle *InjectorStylePlugin::create(const QString &)
{
  inject();
  static QGuiPlatformPlugin defaultGuiPlatform;
  return QStyleFactory::create(defaultGuiPlatform.styleName());
}

QStringList InjectorStylePlugin::keys() const
{
  return QStringList() << QLatin1String("gammaray-injector");
}

void InjectorStylePlugin::inject()
{
  const QByteArray probeDll = qgetenv("GAMMARAY_STYLEINJECTOR_PROBEDLL");
  if (probeDll.isEmpty()) {
    qWarning("No probe DLL specified.");
    return;
  }

#ifdef _WIN32
//TODO: GetLastError and so on for error string
  HMODULE probeDllHandle = LoadLibrary(reinterpret_cast<LPCWSTR>(QString(probeDll).utf16()));
  if (!probeDllHandle) {
    qWarning() << QLatin1String("Failed to load probe dll!");
    return;
  }
#else
  void *probeDllHandle = dlopen(probeDll, RTLD_NOW);
  if (!probeDllHandle) {
    qWarning() << dlerror();
    return;
  }
#endif

  const QByteArray probeFunc = qgetenv("GAMMARAY_STYLEINJECTOR_PROBEFUNC");
  if (probeFunc.isEmpty()) {
    return;
  }
#ifdef _WIN32
//TODO: GetLastError and so on for error string
  FARPROC probeFuncHandle = GetProcAddress(probeDllHandle, QString(probeFunc).toLatin1());
  if (probeFuncHandle) {
    reinterpret_cast<void(*)()>(probeFuncHandle)();
  } else {
    qWarning() << QLatin1String("Error finding probe function!");
  }
#else
  void *probeFuncHandle = dlsym(probeDllHandle, probeFunc);
  if (probeFuncHandle) {
    reinterpret_cast<void(*)()>(probeFuncHandle)();
  } else {
    qWarning() << dlerror();
  }
#endif
}

Q_EXPORT_PLUGIN2(gammaray_injector_style, GammaRay::InjectorStylePlugin)

#include "injectorstyleplugin.moc"
