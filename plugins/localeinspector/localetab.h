/*
  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2018 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_LOCALETAB_H
#define GAMMARAY_LOCALETAB_H

#include <ui/uistatemanager.h>

#include <QWidget>

#include <memory>

namespace GammaRay {

namespace Ui
{
class LocaleTab;
}

class LocaleTab : public QWidget
{
    Q_OBJECT
public:
    explicit LocaleTab(QWidget *parent = nullptr);
    ~LocaleTab() override;

private slots:
    void initSplitterPosition();

private:
    std::unique_ptr<Ui::LocaleTab> ui;
    UIStateManager m_stateManager;
};

}

#endif // GAMMARAY_LOCALETAB_H
