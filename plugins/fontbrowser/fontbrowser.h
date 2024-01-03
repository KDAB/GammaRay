/*
  fontbrowser.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Stephen Kelly <stephen.kelly@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_FONTBROWSER_FONTBROWSER_H
#define GAMMARAY_FONTBROWSER_FONTBROWSER_H

#include <core/toolfactory.h>

#include "fontbrowserserver.h"

#include <QGuiApplication>

namespace GammaRay {
class FontBrowserFactory : public QObject, public StandardToolFactory<QGuiApplication, FontBrowserServer>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolFactory" FILE "gammaray_fontbrowser.json")

public:
    explicit FontBrowserFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }
};
}

#endif // GAMMARAY_FONTBROWSER_H
