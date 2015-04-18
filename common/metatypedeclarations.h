/*
  metatypedeclarations.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2011-2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#include <QMetaMethod>
#include <QMargins>
#include <QPainterPath>
#include <QMatrix4x4>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QGuiApplication>
#include <QOpenGLShader>
#include <QSurface>
#include <QSurfaceFormat>
#endif

Q_DECLARE_METATYPE(Qt::ConnectionType)
Q_DECLARE_METATYPE(Qt::FillRule)
Q_DECLARE_METATYPE(Qt::InputMethodHints)
Q_DECLARE_METATYPE(Qt::MouseButtons)
Q_DECLARE_METATYPE(Qt::TransformationMode)
Q_DECLARE_METATYPE(QPainterPath)
Q_DECLARE_METATYPE(QPolygonF)
Q_DECLARE_METATYPE(QMetaMethod::MethodType)
Q_DECLARE_METATYPE(QMargins)
Q_DECLARE_METATYPE(Qt::WindowType)
Q_DECLARE_METATYPE(Qt::WindowState)
Q_DECLARE_METATYPE(const QMatrix4x4*)

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
Q_DECLARE_METATYPE(QOpenGLShader::ShaderType)
Q_DECLARE_METATYPE(QSurfaceFormat)
Q_DECLARE_METATYPE(QSurface::SurfaceClass)
Q_DECLARE_METATYPE(QSurface::SurfaceType)
#endif

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0) && QT_VERSION < QT_VERSION_CHECK(5, 5, 0)
Q_DECLARE_METATYPE(Qt::ApplicationState)
#endif

#endif
