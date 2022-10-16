/*
  aboutdata.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_ABOUTDATA_H
#define GAMMARAY_ABOUTDATA_H

#include "gammaray_ui_export.h"

#include <QStringList>

namespace GammaRay {
namespace AboutData {
QStringList authors();
QStringList authorsAsHtml();
GAMMARAY_UI_EXPORT QString aboutTitle();
GAMMARAY_UI_EXPORT QString aboutHeader();
GAMMARAY_UI_EXPORT QString aboutAuthors();
GAMMARAY_UI_EXPORT QString aboutFooter();
}
}

#endif // GAMMARAY_ABOUTDATA_H
