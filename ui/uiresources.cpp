/*
  uiresources.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

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

#include "uiresources.h"

#include <QFile>
#include <QPair>
#include <QHash>

using namespace GammaRay;

namespace GammaRay {
namespace UIResources {
typedef QPair<UIResources::IconTheme, QString> PairThemeFileName;
typedef QHash<PairThemeFileName, QString> HashedThemeFilePaths;
static UIResources::IconTheme s_currentTheme = UIResources::Unknown;
static HashedThemeFilePaths s_cachedFilePaths;

UIResources::IconTheme iconTheme()
{
    return s_currentTheme == UIResources::Unknown ? UIResources::Default : s_currentTheme;
}

QString themePath(UIResources::IconTheme theme)
{
    switch (theme) {
    case UIResources::Unknown:
        break;
    case UIResources::Light:
        return QStringLiteral(":/gammaray/icons/ui/light");
    case UIResources::Dark:
        return QStringLiteral(":/gammaray/icons/ui/dark");
    }

    return QString();
}

QString themePath()
{
    return themePath(UIResources::iconTheme());
}

QString themedPath(UIResources::IconTheme theme, const QString &extra)
{
    return QString::fromLatin1("%1/%2").arg(UIResources::themePath(theme), extra);
}

QString themedPath(const QString &extra)
{
    return themedPath(UIResources::iconTheme(), extra);
}

QIcon themedIcon(UIResources::IconTheme theme, const QString &filePath)
{
    const auto pair = PairThemeFileName(theme, filePath);
    auto it = s_cachedFilePaths.find(pair);

    if (it == s_cachedFilePaths.end()) {
        QString candidate = UIResources::themedPath(theme, filePath);
        // Fallback to default theme icons
        if (theme != UIResources::Default && !QFile::exists(candidate)) {
            candidate = UIResources::themedPath(UIResources::Default, filePath);
        }

        it = s_cachedFilePaths.insert(pair, candidate);
        Q_ASSERT_X(QFile::exists(*it), "themedIcon", qPrintable(*it));
    }

    return QIcon(*it);
}
}
}

void UIResources::setIconTheme(UIResources::IconTheme theme)
{
    s_currentTheme = theme;
}

QIcon UIResources::themedIcon(const QString &filePath)
{
    return themedIcon(UIResources::iconTheme(), filePath);
}
