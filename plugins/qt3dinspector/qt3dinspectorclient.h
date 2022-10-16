/*
  qt3dinspectorclient.h

  This file is part of GammaRay, the Qt application inspection and manipulation tool.

  SPDX-FileCopyrightText: 2016-2022 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
  Author: Volker Krause <volker.krause@kdab.com>

  SPDX-License-Identifier: GPL-2.0-or-later OR LicenseRef-KDAB-GammaRay

  Licensees holding valid commercial KDAB GammaRay licenses may use this file in
  accordance with GammaRay Commercial License Agreement provided with the Software.

  Contact info@kdab.com if any conditions of this licensing are not clear to you.
*/

#ifndef GAMMARAY_QT3DINSPECTORCLIENT_H
#define GAMMARAY_QT3DINSPECTORCLIENT_H

#include "qt3dinspectorinterface.h"

namespace GammaRay {
class Qt3DInspectorClient : public Qt3DInspectorInterface
{
    Q_OBJECT
    Q_INTERFACES(GammaRay::Qt3DInspectorInterface)
public:
    explicit Qt3DInspectorClient(QObject *parent = nullptr);
    ~Qt3DInspectorClient();

public slots:
    void selectEngine(int index) override;
};
}

#endif // GAMMARAY_QT3DINSPECTORCLIENT_H
