/*
  libraryutil.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
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
