/*
  uiresources.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_UIRESOURCES_H
#define GAMMARAY_UIRESOURCES_H

#include "gammaray_ui_export.h"

#include <QIcon>
#include <QPixmap>
#include <QImage>

namespace GammaRay {
namespace UIResources {
enum Theme
{
    Unknown,
    Light,
    Dark,
    Default = Light
};

enum ThemeEntryType
{
    Icon,
    Pixmap
};

// Set the current theme to use for UI icons used in actions and toolbars.
GAMMARAY_UI_EXPORT void setTheme(UIResources::Theme theme);

// Relative to UIResources::themePath/icons of the current theme
GAMMARAY_UI_EXPORT QIcon themedIcon(const QString &filePath);

// Relative to UIResources::themePath/pixmaps of the current theme
GAMMARAY_UI_EXPORT QPixmap themedPixmap(const QString &filePath, QWidget *widget);

// Relative to UIResources::themePath/pixmaps of the current theme
GAMMARAY_UI_EXPORT QImage themedImage(const QString &filePath, QWidget *widget);

// Relative to UIResources::themePath/[icons|pixmap] of the current theme
GAMMARAY_UI_EXPORT QString themedFilePath(UIResources::ThemeEntryType type, const QString &filePath, QWidget *widget);

GAMMARAY_UI_EXPORT QImage tintedImage(const QImage &image, const QColor &color);
GAMMARAY_UI_EXPORT QPixmap tintedPixmap(const QImage &image, const QColor &color);

// Deprecated, use setTheme
typedef Theme IconTheme;
inline void setIconTheme(UIResources::IconTheme theme)
{
    setTheme(theme);
}
}
}

#endif // GAMMARAY_UIRESOURCES_H
