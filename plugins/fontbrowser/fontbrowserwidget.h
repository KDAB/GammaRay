/*
  fontbrowserwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>
  Author: Milian Wolff <milian.wolff@kdab.com>

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

#ifndef GAMMARAY_FONTBROWSER_FONTBROWSERWIDGET_H
#define GAMMARAY_FONTBROWSER_FONTBROWSERWIDGET_H

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>

#include <QWidget>

QT_BEGIN_NAMESPACE
class QAbstractItemModel;
class QItemSelection;
QT_END_NAMESPACE

namespace GammaRay {
class FontBrowserInterface;

namespace Ui {
class FontBrowserWidget;
}

class FontBrowserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit FontBrowserWidget(QWidget *parent = nullptr);
    ~FontBrowserWidget() override;

private slots:
    void delayedInit();

private:
    QScopedPointer<Ui::FontBrowserWidget> ui;
    UIStateManager m_stateManager;
    QAbstractItemModel *m_selectedFontModel;
    FontBrowserInterface *m_fontBrowser;
};

class FontBrowserUiFactory : public QObject, public StandardToolUiFactory<FontBrowserWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_fontbrowser.json")
};
}

#endif // GAMMARAY_FONTBROWSERWIDGET_H
