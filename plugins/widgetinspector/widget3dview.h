/*
  widget3dview.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Daniel Vrátil <daniel.vratil@kdab.com>

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

#ifndef WIDGET3DVIEW_H
#define WIDGET3DVIEW_H

#include <QWidget>

namespace GammaRay
{
class Widget3DWindow;
class Widget3DSelectionHelper;

class Widget3DView : public QWidget
{
    Q_OBJECT
public:
    explicit Widget3DView(QWidget *parent = nullptr);
    ~Widget3DView();

protected:
    bool eventFilter(QObject *o, QEvent *e) override;


private:
    void showContextMenu(const QPoint &pos);
    void selectCurrentObject();

    Widget3DWindow *mRenderWindow;
    Widget3DSelectionHelper *mSelectionHelper;

    QPoint mLastRightClick;
};

}

#endif // WIDGET3DVIEW_H
