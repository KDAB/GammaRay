/*
  metatypedeclarations.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2012-2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
  Author: Volker Krause <volker.krause@kdab.com>

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
/**
  @file
  This file is part of the GammaRay Plugin API and declares the various metatypes.

  @brief
  Declares the various metatypes.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_METATYPEDECLARATIONS_H
#define GAMMARAY_METATYPEDECLARATIONS_H

#include <QGraphicsItem>
#include <QGraphicsWidget>
#include <QGraphicsEffect>
#include <QStyle>

Q_DECLARE_METATYPE(Qt::FillRule)
Q_DECLARE_METATYPE(Qt::InputMethodHints)
Q_DECLARE_METATYPE(Qt::MouseButtons)
Q_DECLARE_METATYPE(Qt::TransformationMode)
Q_DECLARE_METATYPE(QGraphicsEffect *)
Q_DECLARE_METATYPE(QGraphicsItemGroup *)
Q_DECLARE_METATYPE(QGraphicsObject *)
Q_DECLARE_METATYPE(QGraphicsWidget *)
Q_DECLARE_METATYPE(QGraphicsItem::CacheMode)
Q_DECLARE_METATYPE(QGraphicsItem::GraphicsItemFlags)
Q_DECLARE_METATYPE(QGraphicsItem::PanelModality)
Q_DECLARE_METATYPE(QGraphicsPixmapItem::ShapeMode)
Q_DECLARE_METATYPE(QPainterPath)
Q_DECLARE_METATYPE(QPolygonF)
Q_DECLARE_METATYPE(const QStyle *)

#endif
