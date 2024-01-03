/*
  selflocator.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_SELFLOCATOR_H
#define GAMMARAY_SELFLOCATOR_H

#include <qglobal.h>

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

namespace GammaRay {

/*! Determines the absolute file path of this DLL. */
namespace SelfLocator {
QString findMe();
}

}

#endif // GAMMARAY_SELFLOCATOR_H
