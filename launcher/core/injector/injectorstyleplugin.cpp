/*
  injectorstyleplugin.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include "injectorstyleplugin.h"

#include <qpa/qplatformtheme.h>
#include <private/qguiapplication_p.h>

#include <QDebug>
#include <QLibrary>
#include <QStyleFactory>

using namespace GammaRay;

QStyle *InjectorStylePlugin::create(const QString &)
{
    inject();
    const QStringList styleNameList
        = QGuiApplicationPrivate::platform_theme->themeHint(
        QPlatformTheme::StyleNames).toStringList();
    for (const QString &styleName : styleNameList) {
        if (QStyle *style = QStyleFactory::create(styleName))
            return style;
    }
    return nullptr;
}

QStringList InjectorStylePlugin::keys() const
{
    return QStringList() << QStringLiteral("gammaray-injector");
}

void InjectorStylePlugin::inject()
{
    const QByteArray probeDllPath = qgetenv("GAMMARAY_STYLEINJECTOR_PROBEDLL");
    if (probeDllPath.isEmpty()) {
        qWarning("No probe DLL specified.");
        return;
    }

    QLibrary probeDll(QString::fromLocal8Bit(probeDllPath));
    probeDll.setLoadHints(QLibrary::ResolveAllSymbolsHint);
    if (!probeDll.load()) {
        qWarning() << "Loading probe DLL failed:" << probeDll.errorString();
        return;
    }

    const QByteArray probeFunc = qgetenv("GAMMARAY_STYLEINJECTOR_PROBEFUNC");
    if (probeFunc.isEmpty()) {
        qWarning("No probe function specified.");
        return;
    }

    QFunctionPointer probeFuncHandle = probeDll.resolve(probeFunc);
    if (probeFuncHandle)
        reinterpret_cast<void (*)()>(probeFuncHandle)();
    else
        qWarning() << "Resolving probe function failed:" << probeDll.errorString();
}
