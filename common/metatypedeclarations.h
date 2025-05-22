/*
  metatypedeclarations.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
#include <QSurfaceFormat>
#endif

Q_DECLARE_METATYPE(Qt::ConnectionType)
Q_DECLARE_METATYPE(QMetaMethod::Access)
Q_DECLARE_METATYPE(QMetaMethod::MethodType)

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
Q_DECLARE_METATYPE(const QMatrix4x4 *)
Q_DECLARE_METATYPE(const QValidator *)

Q_DECLARE_METATYPE(QPainter::CompositionMode)
Q_DECLARE_METATYPE(QPaintEngine::PolygonDrawMode)

Q_DECLARE_METATYPE(QSurfaceFormat)
#endif

#endif
