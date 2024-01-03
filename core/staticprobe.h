/*
  staticprobe.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
#define GAMMARAY_STATIC_INJECT            \
    extern "C" {                          \
    extern void gammaray_install_hooks(); \
    }                                     \
                                          \
    namespace GammaRay {                  \
    class StaticInjector                  \
    {                                     \
    public:                               \
        StaticInjector()                  \
        {                                 \
            gammaray_install_hooks();     \
        }                                 \
    };                                    \
    static StaticInjector staticInjector; \
    }

#endif
