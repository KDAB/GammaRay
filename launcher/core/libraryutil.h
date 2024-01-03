/*
  libraryutil.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_LIBRARYUTIL_H
#define GAMMARAY_LIBRARYUTIL_H

#include <qglobal.h>
#include <QVector>

QT_BEGIN_NAMESPACE
class QByteArray;
QT_END_NAMESPACE

namespace GammaRay {

/*! Utilities for inspecting library dependencies. */
namespace LibraryUtil {

/*! Returns a list of dependencies for @p fileName.
 * @note This is currently only available on Linux.
 */
QVector<QByteArray> dependencies(const QString &fileName);
}

}

#endif // GAMMARAY_LIBRARYUTIL_H
