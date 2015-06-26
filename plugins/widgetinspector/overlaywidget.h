/*
  overlaywidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Tobias Koenig <tobias.koenig@kdab.com>

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

#ifndef GAMMARAY_WIDGETINSPECTOR_OVERLAYWIDGET_H
#define GAMMARAY_WIDGETINSPECTOR_OVERLAYWIDGET_H

#include <QWidget>

namespace GammaRay {

class OverlayWidget : public QWidget
{
  Q_OBJECT

  public:
    OverlayWidget();

    void placeOn(QWidget *widget);

    bool eventFilter(QObject *receiver, QEvent *event) Q_DECL_OVERRIDE;

  protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

  private:
    void resizeOverlay();
    void updatePositions();

    QWidget *m_currentToplevelWidget;
    QWidget *m_currentWidget;
    QRect m_widgetRect;
    QColor m_widgetColor;

    QPainterPath m_layoutPath;
    bool m_drawLayoutOutlineOnly;
};

}

#endif
