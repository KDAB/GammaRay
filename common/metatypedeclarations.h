/*
  metatypedeclarations.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2020 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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
/**
  @file
  This file is part of the GammaRay Plugin API and declares the various metatypes.

  @brief
  Declares the various metatypes.

  @author Volker Krause \<volker.krause@kdab.com\>
*/

#ifndef GAMMARAY_METATYPEDECLARATIONS_H
#define GAMMARAY_METATYPEDECLARATIONS_H

#include <QMetaMethod>
#include <QMetaObject>

#ifdef QT_GUI_LIB
#include <QMargins>
#include <QPainterPath>
#include <QMatrix4x4>
#include <QPainter>
#include <QPaintEngine>
#include <QValidator>

#include <QGuiApplication>
#include <QOpenGLShader>
#include <QSurfaceFormat>
#endif

Q_DECLARE_METATYPE(Qt::ConnectionType)
Q_DECLARE_METATYPE(QMetaMethod::Access)
Q_DECLARE_METATYPE(QMetaMethod::MethodType)
Q_DECLARE_METATYPE(const QMetaObject *)

#include <QTimeZone>
Q_DECLARE_METATYPE(QTimeZone)

#ifdef QT_GUI_LIB // TODO move all this to the GUI support plug-in
Q_DECLARE_METATYPE(Qt::FillRule)
Q_DECLARE_METATYPE(Qt::InputMethodHints)
Q_DECLARE_METATYPE(Qt::MouseButtons)
Q_DECLARE_METATYPE(Qt::TransformationMode)
Q_DECLARE_METATYPE(QPainterPath)
Q_DECLARE_METATYPE(QPolygonF)
Q_DECLARE_METATYPE(QMargins)
Q_DECLARE_METATYPE(Qt::WindowType)
Q_DECLARE_METATYPE(Qt::WindowState)
Q_DECLARE_METATYPE(const QMatrix4x4*)
Q_DECLARE_METATYPE(const QValidator*)

Q_DECLARE_METATYPE(QPainter::CompositionMode)
#if QT_VERSION < QT_VERSION_CHECK(5, 9, 0)
Q_DECLARE_METATYPE(QPainter::RenderHints)
#endif
Q_DECLARE_METATYPE(QPaintEngine::PolygonDrawMode)

#ifndef QT_NO_OPENGL
Q_DECLARE_METATYPE(QOpenGLShader::ShaderType)
#endif
Q_DECLARE_METATYPE(QSurfaceFormat)
#endif

#endif
