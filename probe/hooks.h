/*
  hooks.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_HOOKS_H
#define GAMMARAY_HOOKS_H

#include <qglobal.h>

QT_BEGIN_NAMESPACE
class QObject;
QT_END_NAMESPACE

extern "C" {
extern Q_DECL_EXPORT void gammaray_startup_hook();
extern Q_DECL_EXPORT void gammaray_addObject(QObject *obj);
extern Q_DECL_EXPORT void gammaray_removeObject(QObject *obj);

/** Entry point for startup injection. */
extern Q_DECL_EXPORT void gammaray_probe_inject();

/** Entry point for runtime attaching.
 *  This differs from the above by also attempting to re-send
 *  the server address to the launcher. So only use this if you
 *  are sure there is a launcher ready to receive this information
 *  on the other side.
 */
extern Q_DECL_EXPORT void gammaray_probe_attach();

/** Entry point for static injections. */
extern Q_DECL_EXPORT void gammaray_install_hooks();
}

namespace GammaRay {
namespace Hooks {
/** Returns @c true if we have installed the hooks.
 *  This is useful to avoid loops from preloaded hooks for example.
 */
bool hooksInstalled();

/** Install hooks, either by function overwriting or using qhooks. */
void installHooks();
}
}

#endif
