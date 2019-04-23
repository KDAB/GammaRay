/*
  staticprobe.h

  This file is part of GammaRay, the Qt application inspection and
  manipulation tool.

  Copyright (C) 2015-2019 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
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

#ifndef GAMMARAY_STATIC_PROBE_H
#define GAMMARAY_STATIC_PROBE_H

#include <qplugin.h>

/* Determine which plugins to include.
 * This isn't entirely correct, as it ignores dlopened DSOs for example.
 * However the main motivation for static injection is iOS where we have
 * to statically link everything. If you use dlopen, dynamic injection is
 * the better option anyway.
 */
#ifdef QT_CORE_LIB
Q_IMPORT_PLUGIN(CodecBrowserFactory)
Q_IMPORT_PLUGIN(EventMonitorFactory)
Q_IMPORT_PLUGIN(SignalMonitorFactory)
Q_IMPORT_PLUGIN(StateMachineViewerFactory)
Q_IMPORT_PLUGIN(TimerTopFactory)
Q_IMPORT_PLUGIN(TranslatorInspectorFactory)
#endif
#ifdef QT_GUI_LIB
Q_IMPORT_PLUGIN(FontBrowserFactory)
Q_IMPORT_PLUGIN(GuiSupportFactory)
#endif
#ifdef QT_WIDGETS_LIB
Q_IMPORT_PLUGIN(ActionInspectorFactory)
Q_IMPORT_PLUGIN(SceneInspectorFactory)
Q_IMPORT_PLUGIN(StyleInspectorFactory)
Q_IMPORT_PLUGIN(WidgetInspectorFactory)
#endif
#ifdef QT_QML_LIB
Q_IMPORT_PLUGIN(QmlSupportFactory)
#endif
#ifdef QT_QUICK_LIB
Q_IMPORT_PLUGIN(QuickInspectorFactory)
#endif
#ifdef QT_BLUETOOTH_LIB
Q_IMPORT_PLUGIN(BluetoothFactory)
#endif
#ifdef QT_WEBKIT_LIB
Q_IMPORT_PLUGIN(WebInspectorFactory)
#endif


/** Trigger static injection of the GammaRay probe.
 *  Put this into your main.cpp in global scope.
 */
#define GAMMARAY_STATIC_INJECT \
extern "C" { \
    extern void gammaray_install_hooks(); \
} \
\
namespace GammaRay { \
class StaticInjector { \
public: \
    StaticInjector() { gammaray_install_hooks(); } \
}; \
static StaticInjector staticInjector; \
}

#endif
