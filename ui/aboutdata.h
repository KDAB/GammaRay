/*
  aboutdata.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2014 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
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
