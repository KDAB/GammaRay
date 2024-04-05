/*
  injectorstyleplugin.cpp

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
    const QStringList styleNameList = QGuiApplicationPrivate::platform_theme->themeHint(
                                                                                QPlatformTheme::StyleNames)
                                          .toStringList();
    for (const QString &styleName : styleNameList) {
        if (QStyle *style = QStyleFactory::create(styleName))
            return style;
    }
    return nullptr;
}

QStringList InjectorStylePlugin::keys()
{
    return QStringList() << QStringLiteral("gammaray-injector");
}

void InjectorStylePlugin::inject()
{
    const QString probeDllPath = qEnvironmentVariable("GAMMARAY_STYLEINJECTOR_PROBEDLL");
    if (probeDllPath.isEmpty()) {
        qWarning("No probe DLL specified.");
        return;
    }

    QLibrary probeDll(probeDllPath);
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
