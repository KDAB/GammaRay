/*
  overlaywidget.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2010-2014 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Tobias Koenig <tobias.koenig@kdab.com>

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

    virtual bool eventFilter(QObject *receiver, QEvent *event);

  protected:
    virtual void paintEvent(QPaintEvent *event);

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
