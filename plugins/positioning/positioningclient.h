/*
  positioningclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later

  Contact KDAB at <info@kdab.com> for commercial licensing options.
*/

#ifndef GAMMARAY_POSITIONINGCLIENT_H
#define GAMMARAY_POSITIONINGCLIENT_H

#include "positioninginterface.h"

namespace GammaRay {

class PositioningClient : public PositioningInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::PositioningInterface)
public:
    explicit PositioningClient(QObject *parent = nullptr);
};

}

#endif // GAMMARAY_POSITIONINGCLIENT_H
