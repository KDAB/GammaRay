/*
  kjobtrackerwidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2012-2021 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_KJOBTRACKER_KJOBTRACKERWIDGET_H
#define GAMMARAY_KJOBTRACKER_KJOBTRACKERWIDGET_H

#include <ui/uistatemanager.h>
#include <ui/tooluifactory.h>

#include <QWidget>

namespace GammaRay {
namespace Ui {
class KJobTrackerWidget;
}

class KJobTrackerWidget : public QWidget
{
    Q_OBJECT
public:
    explicit KJobTrackerWidget(QWidget *parent = nullptr);
    ~KJobTrackerWidget() override;

private:
    QScopedPointer<Ui::KJobTrackerWidget> ui;
    UIStateManager m_stateManager;
};

class KJobTrackerUiFactory : public QObject, public StandardToolUiFactory<KJobTrackerWidget>
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::ToolUiFactory)
    Q_PLUGIN_METADATA(IID "com.kdab.GammaRay.ToolUiFactory" FILE "gammaray_kjobtracker.json")
};
}

#endif // GAMMARAY_KJOBTRACKERWIDGET_H
