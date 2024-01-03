/*
  paintanalyzerclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_PAINTANALYZERCLIENT_H
#define GAMMARAY_PAINTANALYZERCLIENT_H

#include <common/paintanalyzerinterface.h>

namespace GammaRay {
class PaintAnalyzerClient : public PaintAnalyzerInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::PaintAnalyzerInterface)
public:
    explicit PaintAnalyzerClient(const QString &name, QObject *parent = nullptr);
};
}

#endif // GAMMARAY_PAINTANALYZERCLIENT_H
