/*
  codecbrowserwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Stephen Kelly <stephen.kelly@kdab.com>

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

#ifndef GAMMARAY_CODECBROWSERWIDGET_CODECBROWSERWIDGET_H
#define GAMMARAY_CODECBROWSERWIDGET_CODECBROWSERWIDGET_H

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>

#include <QWidget>

namespace GammaRay {
namespace Ui {
class CodecBrowserWidget;
}

class CodecBrowserWidget : public QWidget
{
    Q_OBJECT
public:
    explicit CodecBrowserWidget(QWidget *parent = nullptr);
    ~CodecBrowserWidget() override;

private slots:
    void textChanged(const QString &text);

private:
    QScopedPointer<Ui::CodecBrowserWidget> ui;
    UIStateManager m_stateManager;
};

class CodecBrowserUiFactory : public QObject, public StandardToolUiFactory<CodecBrowserWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_codecbrowser.json")
};
}

#endif // GAMMARAY_CODECBROWSERWIDGET_H
