/*
  uiextractor.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2011-2023 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_WIDGETINSPECTOR_UIEXTRACTOR_H
#define GAMMARAY_WIDGETINSPECTOR_UIEXTRACTOR_H

#include <config-gammaray.h>

#ifdef HAVE_QT_DESIGNER
#include <QFormBuilder>

namespace GammaRay {
class UiExtractor : public QFormBuilder
{
protected:
    bool checkProperty(QObject *obj, const QString &prop) const override;
};
}

#endif

#endif // GAMMARAY_UIEXTRACTOR_H
