/*
  wlcompositorclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Giulio Camuffo <giulio.camuffo@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef WLCOMPOSITORCLIENT_H
#define WLCOMPOSITORCLIENT_H

#include "wlcompositorinterface.h"

namespace GammaRay {

class WlCompositorClient : public WlCompositorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::WlCompositorInterface)
public:
    explicit WlCompositorClient(QObject *p);

public slots:
    void connected() override;
    void disconnected() override;
    void setSelectedClient(int index) override;
    void setSelectedResource(uint32_t id) override;
};

}

#endif
