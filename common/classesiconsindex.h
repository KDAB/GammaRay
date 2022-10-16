/*
  classesiconsindex.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_CLASSESICONSINDEX_H
#define GAMMARAY_CLASSESICONSINDEX_H

#include "gammaray_common_export.h"

#include <QHash>
#include <QString>

namespace GammaRay {
namespace ClassesIconsIndex {
typedef QHash<QString, int>::const_iterator ConstIterator;

GAMMARAY_COMMON_EXPORT ConstIterator constBegin();
GAMMARAY_COMMON_EXPORT ConstIterator constEnd();

GAMMARAY_COMMON_EXPORT int iconIdForName(const QString &name);
GAMMARAY_COMMON_EXPORT QString iconNameForId(int id);
}
}

#endif
