/*
  uiresources.cpp

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2014-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QPair>
#include <QHash>
#include <QFile>
#include <QFileInfo>
#include <QWidget>
#include <QApplication>
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
#include <QDesktopWidget>
#endif
#include <QPainter>
#include <QScreen>
#include <QDebug>

using namespace GammaRay;

namespace GammaRay {
namespace UIResources {
struct PairThemeFileName {
    bool operator==(const PairThemeFileName &other) const
    {
        return devicePixelRatio == other.devicePixelRatio &&
                theme == other.theme &&
                filePath == other.filePath;
    }
    bool operator!=(const PairThemeFileName &other) const
    { return !operator==(other); }

    qreal devicePixelRatio;
    UIResources::Theme theme;
    QString filePath;
};
typedef QHash<PairThemeFileName, QString> HashedThemeFilePaths;

UIResources::Theme s_currentTheme = UIResources::Unknown;
QHash<ThemeEntryType, HashedThemeFilePaths> s_cachedFilePaths;

uint qHash(const PairThemeFileName &entry)
{
    uint h1 = ::qHash(entry.devicePixelRatio);
    uint h2 = ::qHash(entry.theme);
    uint h3 = ::qHash(entry.filePath);
    return h1 + h2 + h3;
}

qreal devicePixelRatio(QWidget *widget)
{
    qreal pixelRatio = qApp->devicePixelRatio();

    if (widget) {
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
        const int screenNumber = qMax(0, qApp->desktop()->screenNumber(widget));
        const QScreen *screen = qApp->screens().value(screenNumber);
#else
        const QScreen *screen = widget->screen();
#endif
        pixelRatio = screen->devicePixelRatio();
    }

    return pixelRatio;
}

UIResources::Theme theme()
{
    return s_currentTheme == UIResources::Unknown ? UIResources::Default : s_currentTheme;
}

QString themePath(UIResources::Theme theme)
{
    switch (theme) {
    case UIResources::Unknown:
        break;
    case UIResources::Light:
        return QStringLiteral(":/gammaray/ui/light");
    case UIResources::Dark:
        return QStringLiteral(":/gammaray/ui/dark");
    }

    return QString();
}

QString themePath()
{
    return themePath(UIResources::theme());
}

QString themedPath(UIResources::Theme theme, const QString &extra, QWidget *widget)
{
    QFileInfo candidate(QString::fromLatin1("%1/%2").arg(UIResources::themePath(theme), extra));

    const int dpr = qRound(devicePixelRatio(widget));
    if (dpr > 1) {
        const QString highdpi = QString::fromLatin1("%1/%2@%4x.%3")
                .arg(candidate.path(), candidate.baseName(), candidate.suffix())
                .arg(dpr);
        if (QFile::exists(highdpi))
            candidate.setFile(highdpi);
    }

    return candidate.filePath();
}

QString themedPath(const QString &extra, QWidget *widget)
{
    return themedPath(UIResources::theme(), extra, widget);
}

QString themedFilePath(ThemeEntryType type, UIResources::Theme theme, const QString &filePath, QWidget *widget)
{
    const PairThemeFileName pair = { devicePixelRatio(widget), theme, filePath };
    HashedThemeFilePaths &hash(s_cachedFilePaths[type]);
    auto it = hash.find(pair);

    if (it == hash.end()) {
        const QString iconFilePath = QString::fromLatin1("%1/%2")
                .arg(type == Pixmap ? QStringLiteral("pixmaps") : QStringLiteral("icons"), filePath);
        QString candidate(UIResources::themedPath(theme, iconFilePath, widget));

        // Fallback to default theme file
        if (theme != UIResources::Default && !QFile::exists(candidate)) {
            const QString fallback = UIResources::themedFilePath(type, UIResources::Default, filePath, widget);
            if (QFile::exists(fallback))
                candidate = fallback;
        }

        it = hash.insert(pair, candidate);
        Q_ASSERT_X(QFile::exists(*it), "themedFilePath", qPrintable(*it));
    }

    return *it;
}
}
}

void UIResources::setTheme(UIResources::Theme theme)
{
    s_currentTheme = theme;
}

QIcon UIResources::themedIcon(const QString &filePath)
{
    return QIcon(themedFilePath(Icon, UIResources::theme(), filePath, nullptr));
}

QPixmap UIResources::themedPixmap(const QString &filePath, QWidget *widget)
{
    return QPixmap(themedFilePath(Pixmap, UIResources::theme(), filePath, widget));
}

QImage UIResources::themedImage(const QString &filePath, QWidget *widget)
{
    return QImage(themedFilePath(Pixmap, UIResources::theme(), filePath, widget));
}

QString UIResources::themedFilePath(UIResources::ThemeEntryType type, const QString &filePath, QWidget *widget)
{
    return themedFilePath(type, UIResources::theme(), filePath, widget);
}

QImage UIResources::tintedImage(const QImage &image, const QColor &color)
{
    QImage img(image.size(), QImage::Format_ARGB32_Premultiplied);
    img.setDevicePixelRatio(image.devicePixelRatio());

    QPainter painter(&img);
    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.drawImage(img.rect(), image);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(img.rect(), color);
    return img;
}

QPixmap UIResources::tintedPixmap(const QImage &image, const QColor &color)
{
    return QPixmap::fromImage(tintedImage(image, color));
}
