/*
  painterprofilingreplayer.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2017-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_PAINTERPROFILINGREPLAYER_H
#define GAMMARAY_PAINTERPROFILINGREPLAYER_H

#include "paintbuffer.h"


namespace GammaRay {

class PainterProfilingReplayer
{
public:
    PainterProfilingReplayer();
    ~PainterProfilingReplayer();

    void profile(const PaintBuffer &buffer);
    QVector<double> costs() const;

private:
    QVector<double> m_costs;
};

}

#endif // GAMMARAY_PAINTERPROFILINGREPLAYER_H
