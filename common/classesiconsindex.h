/*
  classesiconsindex.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Filipe Azevedo <filipe.azevedo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
